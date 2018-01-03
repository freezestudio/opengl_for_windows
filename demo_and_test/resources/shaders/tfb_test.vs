//vert tfb_test
#version 430 core

layout( location = 0 ) in vec2 pos;
layout( location = 1 ) in vec2 offset;

//layout ( location = 0 ) out vec2 Offset;
void main()
{    
	//Offset = offset;
    gl_Position = vec4(pos + offset,0.0f,1.0f);
}