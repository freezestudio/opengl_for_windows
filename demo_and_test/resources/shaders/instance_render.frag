//instance_render.frag
#version 430 core
layout(location=0) out vec4 color;
layout(location=0) in vec2 TexCoords;

uniform sampler2D diffuse;

void main()
{
    color = texture(diffuse,TexCoords);
}
