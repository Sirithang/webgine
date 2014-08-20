#pragma once

#include "collection_types.h"

struct RenderKey
{
	union SortingKey
	{
		unsigned long _key;
		struct  
		{
			unsigned char	cameraID;
			unsigned short	shaderID;
			unsigned short	materialID;
		};
	} sortKey;
	
	unsigned short mesh;
};


struct Renderer
{
	//sorted list (according to key) of things to render
	foundation::Array<RenderKey> _renderables;

	Renderer();
};


namespace renderer
{
	void init();

	void addRenderable(RenderKey key);

	void render();
}