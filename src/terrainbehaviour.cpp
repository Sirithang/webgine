#include "terrainbehaviour.h"
#include "mesh.h"
#include "texture.h"
#include "meshrenderer.h"

#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"
#include "mat4x4.h"

#include "std_perlin.h"

#include <emscripten.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <vector>

union RGB
{
	GLuint data;
	struct 
	{
		GLubyte r,g,b,a;
	};
};


alfar::Vector3 faceUVToVector(GLenum face, float U, float V)
{
	alfar::Vector3 ret = {0,0,0};
	switch (face)
	{
	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		ret.x = 1.0f;
		ret.y = ((V - 0.5f) * 2.0f) * (-1.0f);
		ret.z = ((U - 0.5f) * 2.0f) * (-1.0f);
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		ret.x = -1.0f;
		ret.y = ((V - 0.5f) * 2.0f) * (-1.0f);
		ret.z = ((U - 0.5f) * 2.0f) * ( 1.0f);
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		ret.x = ((U - 0.5f) * 2.0f) * ( 1.0f);
		ret.y = 1.0f;
		ret.z = ((V - 0.5f) * 2.0f) * ( 1.0f);
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		ret.x = ((U - 0.5f) * 2.0f) * ( 1.0f);
		ret.y = -1.0f;
		ret.z = ((V - 0.5f) * 2.0f) * (-1.0f);
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		ret.x = ((U - 0.5f) * 2.0f) * ( 1.0f);
		ret.y = ((V - 0.5f) * 2.0f) * (-1.0f);
		ret.z = 1.0f;
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
		ret.x = ((U - 0.5f) * 2.0f) * (-1.0f);
		ret.y = ((V - 0.5f) * 2.0f) * (-1.0f);
		ret.z = -1.0f;
		break;
	default:
		break;
	}

	return alfar::vector3::normalize(ret);
}


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


	spawnGrass(verts, size);

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


	//===================== SKYBOX ==============================

	createSkybox();
}

void TerrainCreator::spawnGrass(InputVertex* inputVert, int size)
{
	//===========================================
	// grass texture generation

	const int grassTexSize = 256;
	const int quarterGrassSize = grassTexSize/4;

	RGB textData[grassTexSize * grassTexSize];

	//memset(textData, 0xFF00FF, sizeof(RGB) * (grassTexSize*grassTexSize));

	for(int i = 0; i < grassTexSize; ++i)
	{
		for(int j = 0; j < grassTexSize; ++j)
		{
			textData[i * grassTexSize + j].data = 0xFF000000;
		}
	}

	const int grassBladesNb = (int)(emscripten_random() * 30) + 20; 
	for(int i = 0; i < grassBladesNb; ++i)
	{
		int startX = grassTexSize / 2 - quarterGrassSize + (emscripten_random() * quarterGrassSize * 2);
		int startY = grassTexSize - 1;
		int width = 6 + emscripten_random() * 6;

		float growX = emscripten_random() * 1.0f - 0.5f;
		float currentX = startX;
		float currentY = startY;

		float growY = -1.0f / sqrtf(growX*growX + 1);
		growX = growX / sqrtf(growX * growX + 1);

		float currentWidth = width;
		float gravity = 0.1f * emscripten_random();

		while(currentY >= 0 && currentX > 0 && currentX < grassTexSize && currentWidth > 0.0)
		{
			int x = (int)roundf(currentX);
			int y = (int)roundf(currentY);

			float xratio = x

			int startW = (int)roundf(currentWidth * 0.5f);
			for(int k = -startW; k <= startW; ++k)
			{
				if(x+k < 0 || x+k >= grassTexSize)
					continue;

				float ratio = (k+startW) / currentWidth;

				textData[y * grassTexSize + (x+k)].r = 128 + (ratio*128);
			}

			currentY += growY;
			currentX += growX;

			growY -= gravity;
			currentWidth -= 0.02f;
		}
	}

	TextureID grassTex = texture::create(GL_TEXTURE_2D);
	texture::uploadData(grassTex, GL_TEXTURE_2D, grassTexSize, grassTexSize, 0, GL_UNSIGNED_BYTE, textData);

	//============================================

	MeshID m = mesh::create();

	const int maxNb = 2000;
	const int number = maxNb;//emscripten_random() * maxNb;
	InputVertex verts[maxNb * 4];
	GLushort idxs[maxNb * 4 * 6];

	int placedCount = 0;
	int currentIdx = 0;

	float radius = 20.0f;

	for(int i = 0; i < number; ++i)
	{
		int x = (size / 2) + (int)(emscripten_random() * radius* 2 - radius);
		int y = (size / 2) + (int)(emscripten_random() * radius* 2 - radius);

		if(x*size + y < size*size)
		{

			int idx = x * size + y;

			if(inputVert[idx].position.y > 0.2 && inputVert[idx].normal.y > 0.9)
			{
				float grassSize = 0.5f + emscripten_random() * 2.5f;
				alfar::Vector4 pos = alfar::vector4::add(inputVert[idx].position, alfar::vector4::create(emscripten_random() * 3.0f - 1.5f, 0, emscripten_random() * 3.0f - 1.5f, 0));
				alfar::Vector2 offsets[4] = {{-1 * grassSize,-1 * grassSize}, {1 * grassSize,-1 * grassSize}, {1 * grassSize,1*grassSize}, {-1*grassSize,1*grassSize}};

				float angle = emscripten_random() * 360.0f;// * 3.14f;
				for(int k = 0; k < 4 ; ++k)
				{
					alfar::Vector3 rotatedR = alfar::vector3::create(offsets[k].x * cos(angle), 0, offsets[k].x * sin(angle));
					alfar::Vector3 offsetRot = alfar::vector3::add(rotatedR, alfar::vector3::create(0,offsets[k].y,0));

					InputVertex v;
					v.position = alfar::vector4::add(pos, alfar::vector4::create(offsetRot, 0.0f));
					v.normal = alfar::vector4::create(0,0,-1, 0);
					v.uv = alfar::vector4::create(offsets[k].x / grassSize, offsets[k].y / grassSize, 0,0);

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
	shader::setFromFile(shad, GL_FRAGMENT_SHADER, "grass.ps");
	shader::link(shad);

	MaterialID grassmat = material::create();
	material::setShader(grassmat, shad);
	//material::setFlag(grassmat, MAT_TRANSPARENT);
	material::setTexture(grassmat, "sTex", grassTex);

	alfar::Vector3 c = alfar::vector3::create(0.0f, 1.0f, 0.0f);
	material::setValue(mat, "uColor", &c, sizeof(alfar::Vector3));
	

	//==========================

	EntityID grassEntity = entity::create();
	MeshRendererID rend = meshrenderer::create(grassEntity);

	meshrenderer::setMesh(rend, m);
	meshrenderer::setMaterial(rend, grassmat);
}

void TerrainCreator::updateSkybox()
{
	const int size = 512;
	alfar::Vector4 skycol = {121.0f/255.0f, 211.0f/255.0f, 224.0f/255.0f, 1.0f};
	alfar::Vector4 groundcol = {240.0f/255.0f, 240.0f/255.0f, 240.0f/255.0f, 1.0f};


	alfar::Vector3 lightdir = alfar::vector3::normalize(alfar::vector3::create(0.7,1.0,0));

	alfar::Vector3 moonpos = alfar::vector3::mul(alfar::vector3::normalize(alfar::vector3::create(0.1,0.1,0.5)), 100.0f);

	//printf("moon pos %f %f %f \n", moonpos.x, moonpos.y, moonpos.z);

	for(int i = 0; i < 6; ++i)
	{
		RGB img[size*size];

		for(int x = 0; x < size; ++x)
		{
			for(int y = 0; y < size; ++y)
			{
				alfar::Vector3 n = faceUVToVector(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, y/(float)size, x/(float)size);

				float d = alfar::vector3::dot(n, alfar::vector3::create(0,1,0));

				d = (d + 1.0f) * 0.5f;//offset horizon
				d = d > 1.0f ? 1.0f : d;

				alfar::Vector4 col = alfar::vector4::lerp(skycol, groundcol, 1.0f-d);

				//alfar::Vector3 cloudIntersection;

				//const float pointDivider = 200.0f;

				//float dist = alfar::vector3::linePlaneIntersection(	
				//	alfar::vector3::create(0,500,0),
				//	alfar::vector3::create(0,1.0f,0),
				//	alfar::vector3::create(0,0,0),
				//	n);
				//
				//if(dist >= 0.0f)
				//{
				//	dist += emscripten_random() * 5.0f;

				//	cloudIntersection = alfar::vector3::mul(n, dist);
				//	float val = stb_perlin_noise3(	
				//		(cloudIntersection.x) * .002f,
				//		(cloudIntersection.y) * .002f,
				//		(cloudIntersection.z) * .002f);


				//	float distWeight = 500.0f / dist;
				//	distWeight = distWeight > 1.0f ? 1.0f : distWeight;
				//	val = (val + 1.0f) * 0.5f * distWeight;

				//	//col = alfar::vector4::lerp(col, alfar::vector4::create(1.0f, 1.0f, 1.0f, 1.0f), val * (dist/pointDivider));

				//	const float strength = 0.4f;// * distWeight;
				//	/*col.x += val * strength;
				//	col.y += val * strength;
				//	col.z += val * strength;*/

				//	col.x = val;
				//	col.y = val;
				//	col.z = val;
				//}

				//==== Moonintersect

				float inters = alfar::vector3::raySphereIntersection(moonpos, 14.0f, alfar::vector3::create(0,0,0), n);
				if(inters >= 0)
				{
					//printf("one intersection\n");
					alfar::Vector3 ptsinter = alfar::vector3::mul(n, inters);

					const float multi = 0.3f;

					float texture = stb_perlin_noise3(ptsinter.x * multi, ptsinter.y * multi, 0);
					float texture2 = stb_perlin_noise3(ptsinter.y * multi, ptsinter.x * multi, 0);

					alfar::Vector3 normal = alfar::vector3::normalize(alfar::vector3::sub(ptsinter, moonpos));

					normal.x += texture;
					normal.y += texture2;

					normal = alfar::vector3::normalize(normal);

					float lighting = alfar::vector3::dot(lightdir, normal);

					col.x += lighting;
				}

				//================

				col = alfar::vector4::clamp(col, 0.0f, 1.0f);

				img[x*size + y].r = 255 * col.x;
				img[x*size + y].g = 255 * col.y;
				img[x*size + y].b = 255 * col.z;
				img[x*size + y].a = 255;
			}
		}

		texture::uploadData(skyboxTex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, size,size,0,GL_UNSIGNED_BYTE, &img[0].r);
	}
}

void TerrainCreator::createSkybox()
{
	int i;
	int j;

	const float radius = 2.0f;
	const int numSlices = 10;
	const int numParallels = numSlices;
	const int numVertices = ( numParallels + 1 ) * ( numSlices + 1 );
	const int numIndices = numParallels * numSlices * 6;
	float angleStep = (2.0f * 3.14f) / ((float) numSlices);


	InputVertex vertices[numVertices];
	GLushort indices[numIndices];

	for ( i = 0; i < numParallels + 1; i++ )
	{
		for ( j = 0; j < numSlices + 1; j++ )
		{
			int vertex = ( i * (numSlices + 1) + j );

			vertices[vertex ].position = alfar::vector4::create(	radius * sinf ( angleStep * (float)i ) * sinf ( angleStep * (float)j ),
																	radius * cosf ( angleStep * (float)i ),
																	radius * sinf ( angleStep * (float)i ) * cosf ( angleStep * (float)j ),
																	1.0f );

			vertices[vertex ].normal = alfar::vector4::create(	vertices[vertex ].position.x/radius,
																vertices[vertex ].position.y/radius,
																vertices[vertex ].position.z/radius,
																0.0f);


			vertices[vertex ].uv = alfar::vector4::create(	(float) j / (float) numSlices,
															( 1.0f - (float) i ) / (float) (numParallels - 1 ),
															0,
															0);
		}
	}

	GLushort *indexBuf = indices;
	for ( i = 0; i < numParallels ; i++ ) 
	{
		for ( j = 0; j < numSlices; j++ )
		{
			*indexBuf++  = i * ( numSlices + 1 ) + j;
			*indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + j;
			*indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + ( j + 1 );

			*indexBuf++ = i * ( numSlices + 1 ) + j;
			*indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + ( j + 1 );
			*indexBuf++ = i * ( numSlices + 1 ) + ( j + 1 );
		}
	}


	//---- SHADER

	ShaderID shad = shader::create();

	shader::setFromFile(shad, GL_VERTEX_SHADER, "skybox.vs");
	shader::setFromFile(shad, GL_FRAGMENT_SHADER, "skybox.ps");
	shader::link(shad);

	skyboxMat = material::create();
	material::setShader(skyboxMat, shad);

	getMaterial(skyboxMat).states.writeDepth = false;
	getMaterial(skyboxMat).states.depthTest = false;
	//material::setFlag(skyboxMat, MAT_TRANSPARENT);
	//----------------------------
	
	MeshID m = mesh::create();
	mesh::upload(m, GL_ARRAY_BUFFER, vertices, numVertices * sizeof(InputVertex));
	mesh::upload(m, GL_ELEMENT_ARRAY_BUFFER, indices, numIndices * sizeof(GLushort));

	skyboxMesh = meshrenderer::create(-1);
	meshrenderer::setMaterial(skyboxMesh, skyboxMat);
	meshrenderer::setMesh(skyboxMesh, m);

	//-----------------------------

	skyboxTex = texture::create(GL_TEXTURE_CUBE_MAP);

	material::setTexture(skyboxMat, "sSkybox", skyboxTex);
	material::setTexture(watMat, "sSkybox", skyboxTex);

	updateSkybox();
}

void TerrainCreator::update(float deltaTime)
{
	accum += deltaTime;
	alfar::Vector4 info = {accum, 0,0,0};
	material::setValue(watMat, "uData", &info, sizeof(alfar::Vector4));

	//updateSkybox();

	RenderKey k = meshrenderer::createRenderKey(getMeshRenderer(skyboxMesh));
	k.transform = alfar::mat4x4::identity();
	k.sortKey.cameraID = 0;
	k.sortKey.layer = 0;
	
	renderer::addRenderable(k);
}