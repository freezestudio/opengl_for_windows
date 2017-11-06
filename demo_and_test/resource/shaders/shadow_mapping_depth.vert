#version 450 core
//shadow_mapping_depth.vert

layout(location=0) in vec3 aPos;
uniform mat4 lightspaceview;
uniform mat4 model;

void main()
{
    gl_Position = lightspaceview * model * vec4(aPos,1.0f);
}