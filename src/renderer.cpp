#include "renderer.h"
#include "memory.h"
#include "array.h"

#include "shader.h"
#include "material.h"
#include "mesh.h"
#include "camera.h"

#include <stdio.h>
#include <stdlib.h>

//need to be a pointer because of array initiation needed AFTER memory_global init
Renderer* gRenderer = NULL;


Renderer::Renderer()
	:_renderables(foundation::memory_globals::default_allocator())
{
	foundation::array::reserve(_renderables, 1024);
}

//==================================

int sortRenderKey(const void* a, const void* b)
{
	return ((RenderKey*)a)->sortKey._key - ((RenderKey*)b)->sortKey._key;
}


//==================================

void renderer::init()
{
	foundation::memory_globals::init();
	gRenderer = new Renderer();
}

void renderer::addRenderable(RenderKey key)
{
	foundation::array::push_back(gRenderer->_renderables, key);
}

void renderer::render()
{
	qsort(gRenderer->_renderables._data, foundation::array::size(gRenderer->_renderables), sizeof(RenderKey), sortRenderKey);

	RenderKey* current = foundation::array::begin(gRenderer->_renderables);
	RenderKey last;
	last.sortKey._key = ~0x0;

	while(current != foundation::array::end(gRenderer->_renderables))
	{
		if(current->sortKey.shaderID != last.sortKey.shaderID)
		{
			shader::bind(current->sortKey.shaderID);

			Camera& c = getCamera(current->sortKey.cameraID);
			shader::setParameter(current->sortKey.shaderID, "uViewMatrix", &c.view);
			shader::setParameter(current->sortKey.shaderID, "uProjectionMatrix", &c.projection);
		}

		if(current->sortKey.materialID != last.sortKey.materialID)
		{
			material::bind(current->sortKey.materialID);
		}

		mesh::bind(current->mesh);
		mesh::draw(current->mesh);

		last = *current;

		current++;
	}

	foundation::array::clear(gRenderer->_renderables);
}