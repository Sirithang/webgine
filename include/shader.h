#pragma once

#include "objectmanager.h"
#include <GLES2/gl2.h>


struct ShaderParameter
{
	int index;
	char name[256];
	unsigned int size;
	GLenum type;
};

struct Shader : public ManagedObject
{
	GLuint _program;
	ShaderParameter _params[16];
	int _paramCount;
};


DECLARE_MANAGED(Shader, 256)

namespace shader
{
	ShaderID create();

	void setShader(ShaderID mat, GLenum type, char* source);
	void link(ShaderID mat);

	void bind(ShaderID mat);

	int getParameterIndex(ShaderID shader, const char* name);

	void setParameter(ShaderID shader, int id, void* value);
	void setParameter(ShaderID mat, const char* name, void* value);

	//---- helper function
	void retrieveUniforms(Shader& mat);
}