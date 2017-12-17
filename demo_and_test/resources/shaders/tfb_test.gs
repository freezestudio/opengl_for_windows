#version 430 core

layout(points) in;
layout(points,max_vertices=2) out;

//1,2,3,4,5
in  float gs_value[];
out float out_value;

void main()
{
    for(int i = 0;i < 2; ++i)
    {
        out_value = gs_value[0];
        EmitVertex();
    }
    EndPrimitive();
}