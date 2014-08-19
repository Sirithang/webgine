attribute vec4 vPosition;
attribute vec4 vNormal;
attribute vec4 vTexcoord;
  
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

varying vec4 oNormal;
varying vec4 oTexcoord;

void main()                
{  
	vec4 pos = vPosition * uViewMatrix;
	pos = pos * uProjectionMatrix;                        
   gl_Position = pos;

   oNormal = vNormal;
   oTexcoord = vTexcoord;
}                          