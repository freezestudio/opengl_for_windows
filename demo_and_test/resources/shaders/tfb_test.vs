#version 430 core

layout(location=0) in float in_value;
out float gs_value;

void main()
{
    gs_value = in_value;
}