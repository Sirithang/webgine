#include "material.h"

#include <algorithm>
#include <stdio.h>

IMPLEMENT_MANAGED(Material)

MaterialID material::create()
{
	MaterialID ret = gMaterialManager.add();
	Material& m = getMaterial(ret);

	m.states.depthTest = true;
	m.states.writeDepth = true;

	m._valuecount = 0;
	m._texturecount = 0;
	m._flags = 0;

	return ret;
}

//------------------------------------------

void material::setShader(MaterialID material, ShaderID shader)
{
	Material& m = getMaterial(material);

	m._shader = shader;
	m._valuecount = 0;
	m._texturecount = 0;
}

//==========================================

//--- value MUST HAVE the size needed for the param name
void material::setValue(MaterialID mat, const char* name, void* value, int size)
{
	Material& m = getMaterial(mat);
	int idx = shader::getParameterIndex(m._shader, name);

	if(idx == -1)
	{
		//printf("Couldn't find : %s in shader\n", name);
		return;
	}

	int existingId = -1;

	for(int i = 0; i < m._valuecount; ++i)
	{
		if(m._values[i].paramIdx == idx)
		{
			existingId = i;
			break;
		}
	}

	if(existingId == -1)
	{//first time we set that param, insert it in array
		m._values[m._valuecount].paramIdx = idx;
		existingId = m._valuecount;
		m._valuecount += 1;
	}

	int correctedSize = std::min<int>(16 * sizeof(float), size);
	memcpy(m._values[existingId].value, value, correctedSize);
}

//=========================================

void material::setTexture(MaterialID mat, const char* name, TextureID tex)
{
	Material& m = getMaterial(mat);
	Texture& t = getTexture(tex);

	int idx = shader::getParameterIndex(m._shader, name);

	if(idx == -1)
	{
		//printf("Couldn't find : %s in shader\n", name);
		return;
	}

	int existingId = -1;

	for(int i = 0; i < m._texturecount; ++i)
	{
		if(m._textures[i].paramIdx == idx)
		{
			existingId = i;
			break;
		}
	}

	if(existingId == -1)
	{//first time we set that param, insert it in array
		m._values[m._texturecount].paramIdx = idx;
		existingId = m._texturecount;
		m._texturecount += 1;
	}

	m._textures[existingId].textureHandle = t._native;//avoid conversion, just store bits
}

//=========================================

void material::setFlag(MaterialID mat, unsigned int flags)
{
	Material& m = getMaterial(mat);
	m._flags = flags;

	switch(flags)
	{
	case MAT_TRANSPARENT:
		m.states.depthTest = true;
		m.states.writeDepth = false;
		break;
	default:
		break;
	}
}

//-----------------------------------------

void material::bind(MaterialID material)
{
	Material& m = getMaterial(material);
	for(int i = 0; i < m._valuecount; ++i)
	{
		shader::setParameter(m._shader, m._values[i].paramIdx, m._values[i].value);
	}

	for(int i = 0; i < m._texturecount; ++i)
	{
		shader::setTexture(m._shader, m._textures[i].paramIdx, m._textures[i].textureHandle, i);
	}
}

//-----------------------------------------

