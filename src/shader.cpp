#include "shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GLES2/gl2.h>

#include <esUtil.h>
#include <string>
#include <iostream>
#include <sstream>

IMPLEMENT_MANAGED(Shader)

ShaderID shader::create()
{
	ShaderID id = gShaderManager.add();

	Shader& mat = getShader(id);

	mat._program = glCreateProgram();

	return id;
}

//===============================

inline void uploadUniform(GLenum type, GLint location, GLuint size, GLfloat* value)
{
	switch(type)
	{
	case GL_FLOAT:
		glUniform1fv(location, size, value);
		break;
	case GL_FLOAT_VEC2:
		glUniform2fv(location, size, value);
		break;
	case GL_FLOAT_VEC3:
		glUniform3fv(location, size, value);
		break;
	case GL_FLOAT_VEC4:
		glUniform4fv(location, size, value);
		break;
	case GL_FLOAT_MAT2:
		glUniformMatrix2fv(location, size, GL_FALSE, value);
		break;
	case GL_FLOAT_MAT3:
		glUniformMatrix3fv(location, size, GL_FALSE, value);
		break;
	case GL_FLOAT_MAT4:
		glUniformMatrix4fv(location, size, GL_FALSE, value);
		break;
	default:
		break;
	}
}

//===============================

//WARNING : that allocate a string, need to be free by caller...
//char* replaceIncludes(const char* original)
//{
//	std::stringstream ss(original);
//	std::string to;
//
//	while(std::getline(ss,to,'\n'))
//	{
//
//		if(to.find("#include"))
//		{
//			to.substr()
//		}
//	}
//}

//===============================

void shader::setFromFile(ShaderID shader, GLenum type, const char* file)
{
	FILE* f = fopen(file, "r");

	// obtain file size:
	fseek (f , 0 , SEEK_END);
	int lSize = ftell (f);
	rewind (f);
	char* shaderStr = (char*) malloc (lSize + 1);
	fread (shaderStr,1,lSize,f);
	shaderStr[lSize] = '\0';

	setShader(shader, type, shaderStr);

	free(shaderStr);
}

//===============================

void shader::setShader(ShaderID mat, GLenum type, char* source)
{
	Shader& m = getShader(mat);

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

void shader::link(ShaderID mat)
{
	Shader& m = getShader(mat);

	// Bind vPosition to attribute 0   
	glBindAttribLocation ( m._program, 0, "vPosition" );
	glBindAttribLocation ( m._program, 1, "vNormal" );
	glBindAttribLocation ( m._program, 2, "vTexcoord" );

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

//-------------------------------------

int shader::getParameterIndex(ShaderID shader, const char* name)
{
	Shader& m = getShader(shader);
	int paramIndex = -1;
	for(int i = 0; i < m._paramCount; ++i)
	{
		if(strcmp(m._params[i].name, name) == 0)
		{
			paramIndex = i;
			break;
		}
	}

	return paramIndex;
}

//--------------------------------------

void shader::setParameter(ShaderID mat, int id, void* value)
{
	if(id != -1)
	{
		Shader& s = getShader(mat);
		uploadUniform(s._params[id].type, s._params[id].index, s._params[id].size, (GLfloat*)value);
	}
}

//--------------------------------------

//size is IN BYTE
void shader::setParameter(ShaderID mat, const char* name, void* value)
{
	setParameter(mat, getParameterIndex(mat, name), value);
}

//---------------------------------------

void shader::setTexture(ShaderID shad, int id, GLuint handle, int slot)
{
	if(id != -1)
	{
		Shader& s = getShader(shad);

		switch(s._params[id].type)
		{
		case GL_SAMPLER_CUBE:
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
			glUniform1i(id, slot);
			break;
		case GL_SAMPLER_2D:
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, handle);
			glUniform1i(id, slot);
			break;
		default:
			break;
		}
	}
}

//--------------------------------------

void shader::retrieveUniforms(Shader& mat)
{
	mat._paramCount = 0;

	GLint nbUniforms;
	glGetProgramiv(mat._program, GL_ACTIVE_UNIFORMS, &nbUniforms);

	for(int i = 0; i < nbUniforms; ++i)
	{
		ShaderParameter param;

		param.index = i;

		GLint lengthname;

		glGetActiveUniform(mat._program, i, 256, &lengthname, (GLint*)&param.size, (GLenum*)&param.type, (GLchar*)&param.name);

		param.index = glGetUniformLocation(mat._program, param.name);

		mat._params[mat._paramCount] = param;
		mat._paramCount++;

		esLogMessage("found uniform %s of size %i \n", param.name, param.size);
	}
}

//--------------------------------------

void shader::bind(ShaderID mat)
{
	Shader& m = getShader(mat);
	glUseProgram ( m._program );
}

//-------------------------------------