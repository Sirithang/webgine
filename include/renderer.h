#pragma once

#include "collection_types.h"
#include "math_types.h"

#include "objectmanager.h"

struct RenderKey
{
	union SortingKey
	{
		unsigned long long _key;
		struct  
		{
			unsigned char	cameraID;
			unsigned char	transparent;

			union
			{//if opaque, everything is sorted by material
				struct 
				{
					unsigned short op_shader;
					unsigned short op_material;
				};
				struct 
				{//in transparent mode, everything is sorted by distance to cam (back to front)
					float transp_dist;
				};
			};
		};
	} sortKey;
	
	alfar::Matrix4x4 transform;

	ID shader;
	ID material;
	ID mesh;
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
	void resetRenderkey(RenderKey& key);

	void render();
}