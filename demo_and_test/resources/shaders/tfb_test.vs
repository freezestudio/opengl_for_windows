#version 430 core

layout(location=0) in float in_value;
out float out_value;

void main()
{
    out_value = pow(in_value,2.0f);
}