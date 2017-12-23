//instance_render.vert
#version 430 core

layout(location=0) in vec3 pos;
layout(location=1) in vec2 uv;
layout(location=2) in mat4 model;
//mat4 bindpoint = 2,3,4,5

layout(location=0) out vec2 TexCoords;

uniform mat4 projection;
uinform mat4 view;

void main()
{
    TexCoords = uv;
    gl_Position = projection * view * model * vec4(pos,1.0f);
}
