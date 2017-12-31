//geom tfb_test
#version 430 core

layout(points) in;
layout(points,max_vertices=1) out;

in VS_OUT
{
    vec2 TexCoords;
    vec3 Offset;
}gs_in[];

layout(location=0)out vec2 TexCoords;

void main()
{
    
    EmitVertex();

    EndPrimitive();
}