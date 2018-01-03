//geom tfb_test
#version 430 core

layout(points) in;
layout(triangle_strip,max_vertices=4) out;

//layout ( location = 0 ) in vec2 Offset[];
layout( location = 0 ) out vec2 TexCoords;

void CreateQuad(vec4 pos)
{
	//左下
	gl_Position = pos + vec4(-0.15f,-0.15f,0.0f,1.0f);
	TexCoords = vec2(0.0f,0.0f);
	EmitVertex();

	//右下
	gl_Position = pos + vec4(0.15f,-0.15f,0.0f,1.0f);
	TexCoords = vec2(1.0f,0.0f);
	EmitVertex();

	//左上
	gl_Position = pos + vec4(-0.15f,0.15f,0.0f,1.0f);
	TexCoords = vec2(0.0f,1.0f);
	EmitVertex();

	//右上
	gl_Position = pos + vec4(0.15f,0.15f,0.0f,1.0f);
	TexCoords = vec2(1.0f,1.0f);
	EmitVertex();

    EndPrimitive();
}

void CreateQuad2(vec2 pos)
{
	//左下
	gl_Position = vec4(pos.x-0.15f,pos.y-0.15f,0.0f,1.0f);
	TexCoords = vec2(0.0f,0.0f);
	EmitVertex();

	//右下
	gl_Position = vec4(pos.x+0.15f,pos.y-0.15f,0.0f,1.0f);
	TexCoords = vec2(1.0f,0.0f);
	EmitVertex();

	//左上
	gl_Position = vec4(pos.x-0.15f,pos.y+0.15f,0.0f,1.0f);
	TexCoords = vec2(0.0f,1.0f);
	EmitVertex();

	//右上
	gl_Position = vec4(pos.x+0.15f,pos.y+0.15f,0.0f,1.0f);
	TexCoords = vec2(1.0f,1.0f);
	EmitVertex();

    EndPrimitive();
}

void main()
{
    CreateQuad(gl_in[0].gl_Position);  
	//CreateQuad2(Offset[0]);
}
