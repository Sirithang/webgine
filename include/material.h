#pragma once

#include "objectmanager.h"
#include <GLES2/gl2.h>


struct MaterialParameter
{
	int index;
	//limit name size but better practice anyway
	char name[256];

	//max value should be 64byte (a matrix)
	//will support more than 16float array later
	unsigned int size;
	float value[16];

	//we have to save it for binding as Ogl use diff func. for each type
	GLenum type;
};

struct Material : public ManagedObject
{
	GLuint _program;

	MaterialParameter _params[16];
	int _paramCount;
};


DECLARE_MANAGED(Material, 256)

namespace material
{
	MaterialID create();

	void setShader(MaterialID mat, GLenum type, char* source);
	void link(MaterialID mat);

	void bind(MaterialID mat);

	void setParameter(MaterialID mat, const char* name, void* value, int size);

	//---- helper function
	void retrieveUniforms(Material& mat);
}