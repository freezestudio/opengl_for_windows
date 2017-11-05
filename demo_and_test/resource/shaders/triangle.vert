#version 430 core
layout(location=0)in vec2 pos;
layout(location=1)in vec2 tex;
out vec2 TexCoords;
void main()
{
    gl_Position = vec4(pos,1.0f,1.0f);
    TexCoords = tex;
}