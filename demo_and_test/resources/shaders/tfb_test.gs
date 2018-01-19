//geom tfb_test
#version 430 core

layout(points) in;
layout(triangle_strip,max_vertices=4) out;

layout( location = 0 ) out vec2 TexCoords;

void CreateQuad(vec4 pos)
{
	//����
	gl_Position = pos + vec4(-1.0f,-1.0f,0.0f,1.0f);
	TexCoords = vec2(0.0f,0.0f);
	EmitVertex();

	//����
	gl_Position = pos + vec4(1.0f,-1.0f,0.0f,1.0f);
	TexCoords = vec2(1.0f,0.0f);
	EmitVertex();

	//����
	gl_Position = pos + vec4(-1.0f,1.0f,0.0f,1.0f);
	TexCoords = vec2(0.0f,1.0f);
	EmitVertex();

	//����
	gl_Position = pos + vec4(1.0f,1.0f,0.0f,1.0f);
	TexCoords = vec2(1.0f,1.0f);
	EmitVertex();

    EndPrimitive();
}

void main()
{
    CreateQuad(gl_in[0].gl_Position);  
}
