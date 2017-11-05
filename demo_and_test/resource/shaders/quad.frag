#version 430 core

in  vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D WoodTexture;
uniform float near_plane;
uniform float far_plane;

float linearize(float depth)
{
    float z = depth * 2.0f -1.0f;
    float linear = (2.0f * near_plane * far_plane)/(far_plane + near_plane - z *(far_plane - near_plane));
    return linear;
}

void main()
{
    float depth_value = texture(WoodTexture,TexCoords).r;
    //FragColor = vec4(vec3(linearize(depth_value)/far_plane),1.0f);
    FragColor = vec4(vec3(depth_value),1.0f);
}