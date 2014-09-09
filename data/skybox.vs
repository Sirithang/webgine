#version 100

attribute vec4 vPosition;
attribute vec4 vNormal;
attribute vec4 vTexcoord;
  
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

varying vec4 oWorldPos;
varying vec3 oNormal;
varying vec4 oTexcoord;

void main()                
{  
	vec4 pos = vPosition;

	oWorldPos = pos;

	pos = pos * mat4(mat3(uViewMatrix));
	pos = pos * uProjectionMatrix;        
	                
	gl_Position = pos;

	oNormal = vNormal.xyz * mat3(uModelMatrix);
	oTexcoord = vTexcoord;
}    