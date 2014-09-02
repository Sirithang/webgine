#include "material.h"

#include <algorithm>
#include <stdio.h>

IMPLEMENT_MANAGED(Material)

MaterialID material::create()
{
	MaterialID ret = gMaterialManager.add();
	Material& m = getMaterial(ret);

	m._count = 0;
	m._flags = 0;

	return ret;
}

//------------------------------------------

void material::setShader(MaterialID material, ShaderID shader)
{
	Material& m = getMaterial(material);

	m._shader = shader;
	m._count = 0;
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

	for(int i = 0; i < m._count; ++i)
	{
		if(m._values[i].paramIdx == idx)
		{
			existingId = i;
			break;
		}
	}

	if(existingId == -1)
	{//first time we set that param, insert it in array
		m._values[m._count].paramIdx = idx;
		existingId = m._count;
		m._count += 1;
	}

	int correctedSize = std::min<int>(16 * sizeof(float), size);
	memcpy(m._values[existingId].value, value, correctedSize);
}

//=========================================

void material::setFlag(MaterialID mat, unsigned int flags)
{
	getMaterial(mat)._flags = flags;
}

//-----------------------------------------

void material::bind(MaterialID material)
{
	Material& m = getMaterial(material);
	for(int i = 0; i < m._count; ++i)
	{
		shader::setParameter(m._shader, m._values[i].paramIdx, m._values[i].value);
	}
}