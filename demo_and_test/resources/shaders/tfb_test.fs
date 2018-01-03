//frag tfb_test
#version 430 core

layout( location = 0 ) in  vec2 TexCoords;
layout( location = 0 ) out vec4 color;

uniform sampler2D image;
void main()
{
    color = texture(image,TexCoords);
}
