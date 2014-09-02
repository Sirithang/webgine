#pragma once

#include "shader.h"

//limited @ 16 float value (4x4 matrix or 4 vec4 etc...)
struct MaterialValue
{
	unsigned char paramIdx;
	float value[16];
};

enum MaterialFlags
{
	MAT_TRANSPARENT = 0x1
};

//This define value to bind to a shader
struct Material : public ManagedObject
{
	ShaderID _shader;

	int _count;
	unsigned int _flags;
	MaterialValue _values[6];
};

DECLARE_MANAGED(Material, 512);

namespace material
{
	MaterialID create();

	void setShader(MaterialID material, ShaderID shader);

	void setValue(MaterialID material, const char* name, void* value, int size);
	void setFlag(MaterialID mat, unsigned int flags);

	void bind(MaterialID material);
}