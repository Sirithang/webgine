#pragma once

#include "shader.h"
#include "texture.h"

//limited @ 16 float value (4x4 matrix or 4 vec4 etc...)
struct MaterialValue
{
	unsigned char paramIdx;
	float value[16];
};

struct TextureValue
{
	unsigned char paramIdx;
	GLuint textureHandle;
};

enum MaterialFlags
{
	MAT_TRANSPARENT = 0x1
};

//Used to toggle state data during rendering
struct StateData
{
	bool writeDepth:1;
	bool depthTest:1;
};

//This define value to bind to a shader
struct Material : public ManagedObject
{
	ShaderID _shader;

	unsigned int _flags;
	StateData states;

	int _valuecount;
	MaterialValue _values[6];
	int _texturecount;
	TextureValue _textures[16];
};

DECLARE_MANAGED(Material, 512);

namespace material
{
	MaterialID create();

	void setShader(MaterialID material, ShaderID shader);

	void setValue(MaterialID material, const char* name, void* value, int size);
	void setTexture(MaterialID mat, const char* name, TextureID tex);

	void setFlag(MaterialID mat, unsigned int flags);

	void bind(MaterialID material);
}