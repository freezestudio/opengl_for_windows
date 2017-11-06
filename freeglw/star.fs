#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube star;

void main()
{
    FragColor = texture(star, TexCoords);
}