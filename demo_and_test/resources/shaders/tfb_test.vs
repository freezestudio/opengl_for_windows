//vert tfb_test
#version 430 core

layout( location = 0 ) in vec4 posuv;
layout( location = 0 ) out vec2 TexCoords;
uniform mat4 model;
void main()
{    
    TexCoords = posuv.zw;
	gl_Position = model * vec4(posuv.xy,0.0f,1.0f);
}