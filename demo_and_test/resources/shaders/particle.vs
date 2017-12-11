#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec2 offset;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS
{
	vec2 TexCoords;
} vs_out;

void main()
{
	vs_out.TexCoords = uv;
	gl_Position = projection * view * model * vec4(pos.x+offset.x,pos.y+offset.y, pos.z, 1.0f);
}