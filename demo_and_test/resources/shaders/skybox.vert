#version 430 core

layout(location=0) in  vec3 pos;
layout(location=1) in mat4 projection;
layout(location=5) in mat4 view;
layout(location=0) out vec3 uvt;

void main()
{
	uvt = pos;
	gl_Position = projection * mat4(mat3(view)) * vec4(pos,1.0f);
}
