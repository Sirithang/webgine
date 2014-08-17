#include <stdlib.h>
#include <stdio.h>
#include "esUtil.h"

#include "material.h"
#include "camera.h"

MaterialID mat;

void Draw ( ESContext *esContext )
{
	camera::update();

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	/*GLfloat vVertices[] = {  0.0f,  1.0f, 0.5f, 
	-0.5f, 1.0f, -0.5f,
	0.5f, 1.0f, -0.5f};*/

	GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f};

	// No clientside arrays, so do this in a webgl-friendly manner
	GLuint vertexPosObject;
	glGenBuffers(1, &vertexPosObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
	glBufferData(GL_ARRAY_BUFFER, 9*4, vVertices, GL_STATIC_DRAW);

	// Set the viewport
	glViewport ( 0, 0, esContext->width, esContext->height );

	// Clear the color buffer
	glClear ( GL_COLOR_BUFFER_BIT );

	material::bind(mat);

	// Load the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
	glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays ( GL_TRIANGLES, 0, 3 );
}

int main(int argc, char** argv)
{
	ESContext esContext;

	esInitContext ( &esContext );
	esCreateWindow ( &esContext, "Hello Triangle", 800, 600, ES_WINDOW_RGB );


	mat = material::create();

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

	material::setShader(mat, GL_VERTEX_SHADER, vShaderStr);
	material::setShader(mat, GL_FRAGMENT_SHADER, fShaderStr);
	material::link(mat);

	EntityID camentity = entity::create();
	CameraID cam = camera::create(camentity);

	/*if ( !Init ( &esContext ) )
	return 0;*/

	esRegisterDrawFunc ( &esContext, Draw );

	esMainLoop ( &esContext );
}