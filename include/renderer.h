#pragma once

#include "collection_types.h"
#include "math_types.h"

#include "objectmanager.h"
#include "material.h"

struct RenderKey
{
	union SortingKey
	{
		unsigned long long _key;
		struct  
		{
			unsigned char	layer;//lower layer  are rendered first
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
	StateData statedat;

	ID shader;
	ID material;
	ID mesh;
};

struct GlobalValue
{
	char name[256];
	float value[16];
};

struct GlobalTexture
{
	char name[256];
	int textureHandle;
};

struct Renderer
{
	GlobalValue _globals[16];
	int _globalNumber;

	//sorted list (according to key) of things to render
	foundation::Array<RenderKey> _renderables;

	Renderer();
};


namespace renderer
{
	void init();

	//set a global value that will be bind to every shader
	void setGlobal(const char* name, float* value, int size);

	void addRenderable(RenderKey key);
	void resetRenderkey(RenderKey& key);

	void render();
}