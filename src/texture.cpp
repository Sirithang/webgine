#include "texture.h"

IMPLEMENT_MANAGED(Texture);


TextureID texture::create(GLenum type)
{
	TextureID ret = gTextureManager.add();

	Texture& t = getTexture(ret);

	glGenTextures(1, &t._native);

	t._target = type;

	return ret;
}

//=================

void texture::uploadData(TextureID tex, GLenum target, int width, int height, int depth, GLenum pixeltype, void* data)
{
	Texture& t = getTexture(tex);

	glBindTexture ( t._target, t._native );

	switch (t._target)
	{
	case GL_TEXTURE_CUBE_MAP:
		glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, pixeltype, data);
		break;
	default:
		break;
	}

	glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}