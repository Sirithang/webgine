attribute vec4 vPosition;
  
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main()                
{  
	vec4 pos = uViewMatrix * vPosition;
	//pos = uProjectionMatrix * pos;                        
   gl_Position = pos;
}                          