#version 330 core

out vec4 color;

in vec2 TexCoord;

uniform sampler2D ourTexture0;
uniform sampler2D ourTexture1;

void main()
{
    color=mix(texture(ourTexture0,TexCoord),texture(ourTexture1,TexCoord),0.2);
}
