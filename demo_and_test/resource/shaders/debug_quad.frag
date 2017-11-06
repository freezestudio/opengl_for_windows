#version 450 core

//debug_quad.frag

out vec4 FragColor;
in  vec2 TexCoords;

uniform sampler2D depthMap;
uniform float nearPlane;
uniform float farPlane;

float linearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
	float linearize = (2.0f * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
	return linearize;
}

void main()
{
    float depthValue = texture(depthMap,TexCoords).r;
	FragColor = vec4(vec3(depthValue),1.0f);
	//FragColor = vec4(vec3(linearizeDepth(depthValue)/farPlane),1.0f) //perspective
}