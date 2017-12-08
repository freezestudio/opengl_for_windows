#version 330 core

in  vec3 WorldPos;
out vec4 FragColor;

uniform samplerCube cube;

void main()
{
	FragColor = texture(cube,WorldPos);
}
