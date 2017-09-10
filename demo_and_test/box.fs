#version 330 core

out vec4 color;

uniform vec3 light_color;
uniform vec3 cube_color;

void main()
{
    color = vec4(light_color * cube_color,1.0f);
}
