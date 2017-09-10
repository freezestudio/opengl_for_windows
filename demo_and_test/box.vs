#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;

out vec3 Normal;
out vec3 Pos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = proj * view * model * vec4(pos, 1.0f);
    Pos = vec3(model * vec4(pos,1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
}
