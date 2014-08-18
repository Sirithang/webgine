#include <stdlib.h>
#include <emscripten.h>
#include <stdio.h>
#include "esUtil.h"

#include "shader.h"
#include "camera.h"
#include "transform.h"
#include "vector3.h"

ShaderID mat;
CameraID cam;

float cls = 0.0f;

void Draw ( void *arg )
{
	ESContext *esContext = (ESContext*)arg;

	camera::update();

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLfloat vVertices[] = {  0.0f,  0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f};

	// No clientside arrays, so do this in a webgl-friendly manner
	GLuint vertexPosObject;
	glGenBuffers(1, &vertexPosObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
	glBufferData(GL_ARRAY_BUFFER, 9*4, vVertices, GL_STATIC_DRAW);

	// Set the viewport
	glViewport ( 0, 0, esContext->width, esContext->height );

	// Clear the color buffer
	glClear ( GL_COLOR_BUFFER_BIT );

	Camera& c = getCamera(cam);

	// Load the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
	glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(0);

	shader::bind(mat);
	shader::setParameter(mat, "uViewMatrix", &c.view);
	shader::setParameter(mat, "uProjectionMatrix", &c.projection);

	glDrawArrays ( GL_TRIANGLES, 0, 3 );
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

	EntityID camentity = entity::create();
	Entity& e = getEntity(camentity);

	transform::setPosition(e._transform, alfar::vector3::create(0,0.5f,0));
	cam = camera::create(camentity);

	/*if ( !Init ( &esContext ) )
	return 0;*/

	//esRegisterDrawFunc ( &esContext, Draw );

	//esMainLoop ( &esContext );
	emscripten_set_main_loop_arg(Draw, &esContext, 0, 1);
}