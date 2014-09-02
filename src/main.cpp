#include <stdlib.h>
#include <emscripten.h>
#include <stdio.h>
#include "esUtil.h"

#include "shader.h"
#include "material.h"
#include "camera.h"
#include "mesh.h"
#include "transform.h"
#include "vector3.h"
#include "meshrenderer.h"
#include "behaviour.h"
#include "input.h"

#include "CameraBehaviour.h"
#include "terrainbehaviour.h"

#include "renderer.h"

CameraID cam;

void Draw ( void *arg )
{
	ESContext *esContext = (ESContext*)arg;

	behaviour::update();
	transform::update();
	camera::update();
	meshrenderer::renderForView(cam);

	glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the viewport
	glViewport ( 0, 0, esContext->width, esContext->height );

	renderer::render();
}

int main(int argc, char** argv)
{
	ESContext esContext;

	esInitContext ( &esContext );
	esCreateWindow ( &esContext, "Hello Triangle", 1024, 768, ES_WINDOW_RGB );

	input::init();
	renderer::init();
	behaviour::init();

	
	//--- Camera ---

	EntityID camchild = entity::create();
	Entity& camchilde = getEntity(camchild);
	cam = camera::create(camchild);

	transform::setPosition(camchilde._transform, alfar::vector3::create(0,15,-100));

	BehaviourID b = behaviour::create(camchild);
	behaviour::setInterface(b, new OrbitCamera());

	//====

	EntityID terrain = entity::create();
	BehaviourID tb = behaviour::create(terrain);
	behaviour::setInterface(tb, new TerrainCreator());


	emscripten_set_main_loop_arg(Draw, &esContext, 0, 1);
}