#pragma once

#include <GLES2/gl2.h>
#include "objectmanager.h"
#include "math_types.h"

////This is used to declare special vertex input.
////TODO
//struct VertexDecl
//{
//	int attribute;
//	int size;
//};

//basic InputVert
struct InputVertex
{
	alfar::Vector4 position;
	alfar::Vector4 normal;
	alfar::Vector4 uv;
};

//describe a mesh, so in effect a gl vertex buffer & gl indice buffer
//This is not a managed type (as it's only a few helper function with 2 Glint)
struct Mesh : public ManagedObject
{
	GLuint _vertices;
	GLuint _indices;
	unsigned int _count;
};

DECLARE_MANAGED(Mesh, 2048)

namespace mesh
{
	MeshID create();
	void upload(MeshID m, GLenum target, void* data, int size);
	void bind(MeshID m);
	void draw(MeshID m);
}