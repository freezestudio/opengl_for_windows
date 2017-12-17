#version 430 core

layout(location=0) in float x;

void main()
{
    gl_Position = vec4(x,0.0f,0.0f,1.0f);
}