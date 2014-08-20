#include "mesh.h"

IMPLEMENT_MANAGED(Mesh)

MeshID mesh::create()
{
	MeshID id = gMeshManager.add();
	Mesh& m = getMesh(id);

	glGenBuffers(1, &m._vertices);
	glGenBuffers(1, &m._indices);
	m._count = 0;

	return id;
}


void mesh::upload(MeshID id, GLenum target, void* data, int size)
{
	Mesh& m = getMesh(id);

	GLuint idx = 0;

	switch(target)
	{
	case GL_ARRAY_BUFFER:
		idx = m._vertices;
		break;
	case GL_ELEMENT_ARRAY_BUFFER:
		idx = m._indices;
		m._count = size / sizeof(GLushort);
		break;
	default:
		break;
	}

	glBindBuffer(target, idx);
	glBufferData(target, size, data, GL_STATIC_DRAW);
}

void mesh::bind(MeshID id)
{
	Mesh& m = getMesh(id);

	glBindBuffer(GL_ARRAY_BUFFER, m._vertices);

	//make that dynamic at some point
	glVertexAttribPointer(0, 4, GL_FLOAT, 0, 3 * sizeof(alfar::Vector4), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, 0, 3 * sizeof(alfar::Vector4), (GLvoid*)sizeof(alfar::Vector4));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_FLOAT, 0, 3 * sizeof(alfar::Vector4), (GLvoid*)(sizeof(alfar::Vector4)*2));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m._indices);
}

void mesh::draw(MeshID id)
{
	Mesh& m = getMesh(id);
	glDrawElements ( GL_TRIANGLES, m._count, GL_UNSIGNED_SHORT, 0 );
}