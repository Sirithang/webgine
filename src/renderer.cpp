#include "renderer.h"
#include "memory.h"
#include "array.h"

#include "shader.h"
#include "material.h"
#include "mesh.h"
#include "camera.h"
#include "transform.h"

#include "vector4.h"

#include <stdio.h>
#include <stdlib.h>

//need to be a pointer because of array initiation needed AFTER memory_global init
Renderer* gRenderer = NULL;


Renderer::Renderer()
	:_renderables(foundation::memory_globals::default_allocator())
{
	foundation::array::reserve(_renderables, 1024);
	_globalNumber = 0;
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

void renderer::setGlobal(const char* name, float* value, int size)
{
	int found = -1;
	for(int i = 0; i < gRenderer->_globalNumber; ++i)
	{
		if(strcmp(gRenderer->_globals[i].name, name) == 0)
		{
			found = i;
				break;
		}
	}

	if(found == -1)
	{
		found = gRenderer->_globalNumber;
		strncpy(gRenderer->_globals[found].name, name, 256);

		gRenderer->_globalNumber += 1;
	}

	memcpy(gRenderer->_globals[found].value, value, size);
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

	key.statedat.depthTest = true;
	key.statedat.writeDepth = true;
	key.sortKey.transparent = 0;
}

//------------------

inline void toggleStates(const StateData& prev, const StateData& current, bool testprev = true)
{
	if(!testprev || prev.depthTest != current.depthTest)
	{
		if(current.depthTest)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

	}

	if(!testprev && current.writeDepth != prev .writeDepth)
	{
		glDepthMask(current.writeDepth? GL_TRUE : GL_FALSE);
	}
}

//------------------

void renderer::render()
{
	qsort(gRenderer->_renderables._data, foundation::array::size(gRenderer->_renderables), sizeof(RenderKey), sortRenderKey);

	RenderKey* current = foundation::array::begin(gRenderer->_renderables);
	RenderKey last;
	resetRenderkey(last);

	StateData prevData;
	if(current != foundation::array::end(gRenderer->_renderables))
	{
		prevData = current->statedat;
		toggleStates(prevData, prevData, false);
	}
	
	
	glDisable(GL_BLEND);

	while(current != foundation::array::end(gRenderer->_renderables))
	{
		toggleStates(prevData, current->statedat);
		prevData = current->statedat;
	
		if(current->sortKey.transparent != last.sortKey.transparent)
		{//we just switch to transparent rendering, disable depth write & simple blending (ATM, TODO configurable per mat)
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		if(current->shader != last.shader)
		{
			shader::bind(current->shader); 

			Camera& c = getCamera(current->sortKey.cameraID);
			shader::setParameter(current->shader, "uViewMatrix", &c.view);
			shader::setParameter(current->shader, "uProjectionMatrix", &c.projection);

			alfar::Vector4 viewDir = alfar::vector4::create(transform::getWorldPosition(c._tn),0);

			shader::setParameter(current->shader, "uCameraPos", &viewDir);

			//rebind every global
			for(int i = 0; i < gRenderer->_globalNumber; ++i)
			{
				shader::setParameter(current->shader, gRenderer->_globals[i].name, gRenderer->_globals[i].value);
			}
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