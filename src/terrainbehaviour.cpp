#include "terrainbehaviour.h"
#include "mesh.h"
#include "meshrenderer.h"

#include "vector3.h"
#include "vector4.h"

#include "std_perlin.h"

#include <emscripten.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <vector>

TerrainCreator::TerrainCreator()
{
	accum = 0;
}

void TerrainCreator::created()
{
	//---- SHADER

	ShaderID shad = shader::create();

	shader::setFromFile(shad, GL_VERTEX_SHADER, "basepassvertex.vs");
	shader::setFromFile(shad, GL_FRAGMENT_SHADER, "terrainpass.ps");
	shader::link(shad);

	alfar::Vector3 c = alfar::vector3::create(0.5f, 1.0f, 0.6f);

	mat = material::create();
	material::setShader(mat, shad);
	material::setValue(mat, "uColor", &c, sizeof(alfar::Vector3));

	//----------------------------


	MeshID m = mesh::create();

	const int size = 256;
	const float spacing = 4.0f;

	InputVertex verts[size*size];

	const int nbIndices = (size-1)*(size-1)*6;
	GLushort indices[nbIndices];

	memset(indices, 0, nbIndices * sizeof(GLushort));

	alfar::Vector4 lowerCorner = alfar::vector4::create(-(size*0.5f*spacing), -0.3f, -(size*0.5f*spacing), 1.0f);
	int currentIdx = 0;

	float multiX = emscripten_random() * 4.0f;
	float multiY = emscripten_random() * 4.0f;
	float multiZ = emscripten_random() * 20.0f;

	for(int i = 0; i < size; ++i)
	{
		for(int j = 0; j < size; ++j)
		{
			int dx = i - size/2;
			int dy = j - size/2;

			float val = stb_perlin_noise3(i/(float)size * multiX ,j/(float)size * multiY ,0);

			verts[i*size + j].position = alfar::vector4::add(lowerCorner, alfar::vector4::create(i * spacing, (val + 0.2f) * multiZ , j * spacing ,0.0f));
			verts[i*size + j].uv = alfar::vector4::create(i / (float)size * spacing, j / (float)size * spacing, 0.0f, 1.0f);

			if(i > 0 && j > 0)
			{
				indices[currentIdx+0] = (i-1)*size + (j-1);
				indices[currentIdx+1] = (i-1)*size + (j-0);
				indices[currentIdx+2] = (i-0)*size + (j-1);

				indices[currentIdx+3] = (i-1)*size + (j-0);
				indices[currentIdx+4] = (i-0)*size + (j-0);
				indices[currentIdx+5] = (i-0)*size + (j-1);

				currentIdx += 6;
			}
		}
	}

	//-- TWO PASS : this is sucky but easier atm

	for(int i = 0; i < size; ++i)
	{
		for(int j = 0; j < size; ++j)
		{
			//-- normals
			if(i > 0 && i < size - 1 && j > 0 && j < size - 1)
			{
				alfar::Vector4 v0 = verts[i*size + j].position;
				alfar::Vector4 v1 = verts[(i-1)*size + j].position;
				alfar::Vector4 v2 = verts[(i+1)*size + j].position;

				alfar::Vector4 v3 = verts[i*size + (j-1)].position;
				alfar::Vector4 v4 = verts[i*size + (j+1)].position;

				alfar::Vector3 p1 = alfar::vector4::toVec3(alfar::vector4::sub(v1,v0));
				alfar::Vector3 p2 = alfar::vector4::toVec3(alfar::vector4::sub(v2,v0));
				alfar::Vector3 p3 = alfar::vector4::toVec3(alfar::vector4::sub(v3,v0));
				alfar::Vector3 p4 = alfar::vector4::toVec3(alfar::vector4::sub(v4,v0));

				alfar::Vector3 n1 = alfar::vector3::cross(p3,p1);
				alfar::Vector3 n2 = alfar::vector3::cross(p2,p3);
				alfar::Vector3 n3 = alfar::vector3::cross(p4,p2);
				alfar::Vector3 n4 = alfar::vector3::cross(p1,p4);

				alfar::Vector3 n = alfar::vector3::add(n1,n2);
				n = alfar::vector3::add(n,n3);
				n = alfar::vector3::add(n,n4);
				n = alfar::vector3::normalize(n);

				verts[i*size + j].normal = alfar::vector4::create(n, 0.0f);
			}
		}
	}

	mesh::upload(m, GL_ARRAY_BUFFER, verts, (size*size)*sizeof(InputVertex));
	mesh::upload(m, GL_ELEMENT_ARRAY_BUFFER, indices, currentIdx * sizeof(GLushort));


	//spawnGrass(verts, size);

	MeshRendererID r = meshrenderer::create(this->_entity);
	meshrenderer::setMesh(r, m);
	meshrenderer::setMaterial(r, mat);


	//========================================
	// --- WATER PLANE
	EntityID waterent = entity::create();
	MeshID sqr = mesh::create();

	InputVertex vert[4] =	
	{{{-size * spacing, 0,-size * spacing,1},{0,1,0,0},{0,0,0,0}},
	{{ -size * spacing, 0, size * spacing,1},{0,1,0,0},{0,1,0,0}},
	{{  size * spacing, 0, size * spacing,1},{0,1,0,0},{1,1,0,0}},
	{{  size * spacing, 0,-size * spacing,1},{0,1,0,0},{1,0,0,0}}};

	GLushort idx[6] = {0,1,2,0,2,3};

	mesh::upload(sqr, GL_ARRAY_BUFFER, vert, 4 * sizeof(InputVertex));
	mesh::upload(sqr, GL_ELEMENT_ARRAY_BUFFER, idx, 6 * sizeof(GLushort));

	MeshRendererID rsqr = meshrenderer::create(waterent);
	meshrenderer::setMesh(rsqr, sqr);

	transform::setPosition(getEntity(waterent)._transform, alfar::vector3::create(0,-0.5,0));

	// == Water material
	ShaderID watershader = shader::create();

	shader::setFromFile(watershader, GL_VERTEX_SHADER, "basepassvertex.vs");
	shader::setFromFile(watershader, GL_FRAGMENT_SHADER, "water.ps");
	shader::link(watershader);

	watMat = material::create();
	material::setShader(watMat, watershader);
	material::setFlag(watMat, MAT_TRANSPARENT);

	meshrenderer::setMaterial(rsqr, watMat);
}

void TerrainCreator::spawnGrass(InputVertex* inputVert, int size)
{
	MeshID m = mesh::create();

	const int maxNb = 2000;
	const int number = 1000;//emscripten_random() * maxNb;
	InputVertex verts[maxNb * 4];
	GLushort idxs[maxNb * 4 * 6];

	int placedCount = 0;
	int currentIdx = 0;

	for(int i = 0; i < number; ++i)
	{
		int x = (int)(emscripten_random() * size);
		int y = (int)(emscripten_random() * size);

		if(x*size + y < size*size)
		{

			int idx = x * size + y;

			if(inputVert[idx].position.y > 0.2 && inputVert[idx].normal.y > 0.9)
			{
				alfar::Vector4 pos = alfar::vector4::add(inputVert[idx].position, alfar::vector4::create(emscripten_random()-0.5f, 0, emscripten_random()-0.5f, 0));
				alfar::Vector2 offsets[4] = {{-1,-1}, {1,-1}, {1,1}, {-1,1}};
				for(int k = 0; k < 4 ; ++k)
				{
					InputVertex v;
					v.position = alfar::vector4::add(pos, alfar::vector4::create(offsets[k].x, offsets[k].y, 0, 0));
					v.normal = alfar::vector4::create(0,0,-1, 0);
					v.uv = alfar::vector4::create(offsets[k].x, offsets[k].y, 0,0);

					verts[placedCount + k] = v;
				}

				idxs[currentIdx+0] = (placedCount+0);
				idxs[currentIdx+1] = (placedCount+1);
				idxs[currentIdx+2] = (placedCount+2);
				idxs[currentIdx+3] = (placedCount+0);
				idxs[currentIdx+4] = (placedCount+2);
				idxs[currentIdx+5] = (placedCount+3);

				placedCount += 4;
				currentIdx += 6;
			}
		}
	}

	mesh::upload(m, GL_ARRAY_BUFFER, verts, placedCount * sizeof(InputVertex));
	mesh::upload(m, GL_ELEMENT_ARRAY_BUFFER, idxs, currentIdx * sizeof(GLushort));

	//===

	//---- SHADER & MAT

	ShaderID shad = shader::create();

	shader::setFromFile(shad, GL_VERTEX_SHADER, "basepassvertex.vs");
	shader::setFromFile(shad, GL_FRAGMENT_SHADER, "basepasspixel.ps");
	shader::link(shad);

	MaterialID grassmat = material::create();
	material::setShader(grassmat, shad);

	alfar::Vector3 c = alfar::vector3::create(0.0f, 1.0f, 0.0f);
	material::setValue(mat, "uColor", &c, sizeof(alfar::Vector3));

	//==========================

	EntityID grassEntity = entity::create();
	MeshRendererID rend = meshrenderer::create(grassEntity);

	meshrenderer::setMesh(rend, m);
	meshrenderer::setMaterial(rend, grassmat);
}

void TerrainCreator::update(float deltaTime)
{
	accum += deltaTime;
	alfar::Vector4 info = {accum, 0,0,0};
	material::setValue(watMat, "uData", &info, sizeof(alfar::Vector4));
}