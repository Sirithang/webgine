#include <stdlib.h>
#include <emscripten.h>
#include <stdio.h>
#include "esUtil.h"

#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "transform.h"
#include "vector3.h"

ShaderID mat;
CameraID cam;
EntityID camentity;
Mesh m;

int dir = 1;
float cls = 0.0f;

void Draw ( void *arg )
{
	ESContext *esContext = (ESContext*)arg;

	cls += 0.01f * dir;
	if(cls > 1.0f)
	{
		cls = 1.0f;
		dir = -dir;
	}
	else if(cls < -1.0f)
	{
		cls = -1.0f;
		dir = -dir;
	}
	Entity& e = getEntity(camentity);
	transform::setPosition(e._transform, alfar::vector3::create(0, cls, -2));

	camera::update();

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set the viewport
	glViewport ( 0, 0, esContext->width, esContext->height );

	// Clear the color buffer
	glClear ( GL_COLOR_BUFFER_BIT );

	Camera& c = getCamera(cam);

	shader::bind(mat);
	shader::setParameter(mat, "uViewMatrix", &c.view);
	shader::setParameter(mat, "uProjectionMatrix", &c.projection);

	mesh::bind(m);
	mesh::draw(m);
}

int main(int argc, char** argv)
{
	ESContext esContext;

	esInitContext ( &esContext );
	esCreateWindow ( &esContext, "Hello Triangle", 800, 600, ES_WINDOW_RGB );

	mat = shader::create();

	FILE* f = fopen("basepassvertex.vs", "r");

	// obtain file size:
	fseek (f , 0 , SEEK_END);
	int lSize = ftell (f);
	rewind (f);
	char* vShaderStr = (char*) malloc (lSize + 1);
	fread (vShaderStr,1,lSize,f);
	vShaderStr[lSize] = '\0';

	//char fShaderStr[] =  
	//	"precision mediump float;\n"\
	//	"void main()                                  \n"
	//	"{                                            \n"
	//	"  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
	//	"}                                            \n";

	fclose(f);
	f = fopen("basepasspixel.ps", "r");
	fseek (f , 0 , SEEK_END);
	lSize = ftell (f);
	rewind (f);
	char* fShaderStr = (char*) malloc (lSize + 1);
	fread (fShaderStr,1,lSize,f);
	fShaderStr[lSize] = '\0';

	shader::setShader(mat, GL_VERTEX_SHADER, vShaderStr);
	shader::setShader(mat, GL_FRAGMENT_SHADER, fShaderStr);
	shader::link(mat);

	camentity = entity::create();
	Entity& e = getEntity(camentity);

	transform::setPosition(e._transform, alfar::vector3::create(0,0,0));
	cam = camera::create(camentity);

	GLushort indices[] = {0,1,2,0,2,3};
	InputVertex verts[] = 
	{
		{{-0.5f, -0.5f, 0.5f,1.0f},{0,0,0,0},{0,0,0,0}},
		{{ 0.5f, -0.5f, 0.5f,1.0f},{0,0,0,0},{0,0,0,0}},
		{{ 0.5f,  0.5f, 0.5f,1.0f},{0,0,0,0},{0,0,0,0}},
		{{-0.5f,  0.5f, 0.5f,1.0f},{0,0,0,0},{0,0,0,0}}
	};

	mesh::create(m);
	mesh::upload(m, GL_ARRAY_BUFFER, verts, 4 * sizeof(InputVertex));
	mesh::upload(m, GL_ELEMENT_ARRAY_BUFFER, indices, 6 * sizeof(GLushort));

	emscripten_set_main_loop_arg(Draw, &esContext, 0, 1);
}