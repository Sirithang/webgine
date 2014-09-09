#pragma once

#include "objectmanager.h"
#include <GLES2/gl2.h>

struct Texture : public ManagedObject
{
	GLuint _native; 
	GLenum _target;
};

DECLARE_MANAGED(Texture, 512)

namespace texture
{
	TextureID create(GLenum type);

	void uploadData(TextureID tex, GLenum target, int width, int height, int depth, GLenum pixeltype, void* data);
}