#include "material.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GLES2/gl2.h>

#include <esUtil.h>

IMPLEMENT_MANAGED(Material)

MaterialID material::create()
{
	MaterialID id = gMaterialManager.add();

	Material& mat = getMaterial(id);

	mat._program = glCreateProgram();

	return id;
}

//===============================

int GLTypeToFloatNumber(GLenum type)
{
	switch(type)
	{
	case GL_FLOAT:
		return 1;
	case GL_FLOAT_VEC2:
		return 2;
	case GL_FLOAT_VEC3:
		return 3;
	case GL_FLOAT_VEC4:
		return 4;
	case GL_FLOAT_MAT2:
		return 4;
	case GL_FLOAT_MAT3:
		return 9;
	case GL_FLOAT_MAT4:
		return 16;
	default:
		break;
	}

	return 0;
}

//===============================

void material::setShader(MaterialID mat, GLenum type, char* source)
{
	Material& m = getMaterial(mat);

	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader ( type );

	if ( shader == 0 )
	{
		printf("ERROR CREATING SHADER");
		return;
	}

	// Load the shader source
	glShaderSource ( shader, 1, (const GLchar**)&source, 0 );

	// Compile the shader
	glCompileShader ( shader );

	// Check the compile status
	glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

	if ( !compiled ) 
	{
		GLint infoLen = 0;

		glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

		if ( infoLen > 1 )
		{
			char* infoLog = (char*)malloc (sizeof(char) * infoLen );

			glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
			printf ( "Error compiling shader:\n%s\n", infoLog );            

			free ( infoLog );
		}

		glDeleteShader ( shader );
		return;
	}

	glAttachShader ( m._program , shader );
}

//------------------------------------

void material::link(MaterialID mat)
{
	Material& m = getMaterial(mat);

	// Bind vPosition to attribute 0   
	glBindAttribLocation ( m._program, 0, "vPosition" );
	glLinkProgram ( m._program );

	 GLint linked;
	// Check the link status
	glGetProgramiv ( m._program, GL_LINK_STATUS, &linked );

	if ( !linked ) 
	{
		GLint infoLen = 0;

		glGetProgramiv ( m._program, GL_INFO_LOG_LENGTH, &infoLen );

		if ( infoLen > 1 )
		{
			char* infoLog = (char*)malloc (sizeof(char) * infoLen );

			glGetProgramInfoLog ( m._program, infoLen, NULL, infoLog );
			printf ( "Error linking program:\n%s\n", infoLog );            

			free ( infoLog );
		}

		glDeleteProgram ( m._program );
		return;
	}

	retrieveUniforms(m);
}

//--------------------------------------

//size is IN BYTE
void material::setParameter(MaterialID mat, const char* name, void* value, int size)
{
	Material& m = getMaterial(mat);
	int paramIndex = -1;
	for(int i = 0; i < m._paramCount; ++i)
	{
		if(strcmp(m._params[i].name, name) == 0)
		{
			paramIndex = i;
			break;
		}
	}

	if(paramIndex == -1)
	{
		esLogMessage("param %s not found\n", name);
		return;
	}

	if(m._params[paramIndex].size < size)
	{
		esLogMessage("size bigger than param size\n");
		return;
	}

	memcpy(&m._params[paramIndex].value[0], value, size);
}

//--------------------------------------

void material::retrieveUniforms(Material& mat)
{
	mat._paramCount = 0;

	GLint nbUniforms;
	glGetProgramiv(mat._program, GL_ACTIVE_UNIFORMS, &nbUniforms);

	esLogMessage("nb uniform : %i \n", nbUniforms);

	for(int i = 0; i < nbUniforms; ++i)
	{
		MaterialParameter param;

		memset(&param.value[0], 0, 16*sizeof(float));

		param.index = i;

		GLint lengthname;

		glGetActiveUniform(mat._program, i, 256, &lengthname, (GLint*)&param.size, (GLenum*)&param.type, (GLchar*)&param.name);

		mat._params[mat._paramCount] = param;
		mat._paramCount++;

		esLogMessage("found uniform %s of size %i \n", param.name, param.size);
	}
}

//--------------------------------------

void material::bind(MaterialID mat)
{
	Material& m = getMaterial(mat);
	glUseProgram ( m._program );

	for(int i = 0; i < m._paramCount; ++i)
	{
		MaterialParameter param = m._params[i];
		glUniform1fv(param.index, GLTypeToFloatNumber(param.type) * param.size, param.value);
	}
}

//-------------------------------------