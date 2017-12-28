#version 430 core

layout(location=0) out vec4 color;
layout(location=0) in  vec3 uvt;

uniform samplerCube skybox;

void main()
{
	color = texture(skybox,uvt);
}
