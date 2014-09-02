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

	glEnable(GL_DEPTH_TEST);
}

void renderer::addRenderable(RenderKey key)
{
	foundation::array::push_back(gRenderer->_renderables, key);
}

void renderer::resetRenderkey(RenderKey& key)
{
	key.mesh = -1;
	key.material = -1;
	key.shader = -1;
	key.sortKey._key =  ~(0x0ULL);

	key.sortKey.transparent = 0;
}

void renderer::render()
{
	qsort(gRenderer->_renderables._data, foundation::array::size(gRenderer->_renderables), sizeof(RenderKey), sortRenderKey);

	RenderKey* current = foundation::array::begin(gRenderer->_renderables);
	RenderKey last;
	resetRenderkey(last);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	while(current != foundation::array::end(gRenderer->_renderables))
	{
		if(current->sortKey.transparent != last.sortKey.transparent)
		{//we just switch to transparent rendering, disable depth write & simple blending (ATM, TODO configurable per mat)
			glDepthMask(GL_FALSE);
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		
		if(current->shader != last.shader)
		{
			shader::bind(current->shader);

			Camera& c = getCamera(current->sortKey.cameraID);
			shader::setParameter(current->shader, "uViewMatrix", &c.view);
			shader::setParameter(current->shader, "uProjectionMatrix", &c.projection);
		}

		if(current->material != last.material)
		{
			material::bind(current->material);
		}

		shader::setParameter(current->shader, "uModelMatrix", &current->transform);

		mesh::bind(current->mesh);
		mesh::draw(current->mesh);

		last = *current;

		current++;
	}

	foundation::array::clear(gRenderer->_renderables);
}