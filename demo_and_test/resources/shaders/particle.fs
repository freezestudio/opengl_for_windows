#version 430 core

out vec4 FragColor;

in VS
{
	vec2 TexCoords;
} fs_in;

uniform sampler2D sprite;

void main()
{
	FragColor = texture(sprite,fs_in.TexCoords);
}