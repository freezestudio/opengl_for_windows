//vert tfb_test
#version 430 core

layout(location=0) in vec3 pos;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 offset;

out VS_OUT
{
    vec2 TexCoords;
    vec3 Offset;
} vs_out;

void main()
{
    vs_out.TexCoords = uv;
    vs_out.Offset = offset;
    gl_Position = vec4(pos,1.0f);
}