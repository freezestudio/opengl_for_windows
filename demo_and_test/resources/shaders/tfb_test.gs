#version 430 core

layout(points) in;
layout(points,max_vertices=1) out;

//1,2,3,4,5
in  float gs_value[];
out float out_value;

void main()
{
    out_value = gs_value[0] * 0.2;
    EmitVertex();

    EndPrimitive();
}