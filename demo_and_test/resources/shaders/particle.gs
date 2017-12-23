#version 430 core

layout(points) in;                                                                  
layout(points, max_vertices = 30) out;                                             
  
in VS
{
    vec3  Position;                                                                
    vec3  Velocity;                                                                
    float Age;
} gs_in[];
                                                                     
out GS
{
    vec3  Position;                                                                 
    vec3  Velocity;                                                                 
    float Age;  
} gs_out;
                                                                                  
uniform float gDeltaTime;                                                     
uniform float gTime;

uniform sampler1D gRandomTexture;                                              
                                                                                  
vec3 GetRandomDir(float TexCoord)                                                   
{                                                                                   
     vec3 Dir = texture(gRandomTexture, TexCoord).xyz; 
     Dir -= vec3(0.5f); 
     return Dir;                                                                    
}                                                                                   
                                                                                    
void main()                                                                         
{ 
    float age = gs_in[0].Age + gDeltaTime;
    vec3 dir = GetRandomDir(gTime*0.01f);
    for(int i=0;i<30;++i)
    {
        if(gs_in[0].Age >=2.0f || gs_in[0].Position.y>=1.0f)
        {
            gs_out.Position = dir * gs_in[0].Velocity;
            gs_out.Velocity = gs_in[0].Velocity;
            gs_out.Age = 0.0f;
        }
        else
        {
            gs_out.Position = gs_in[0].Position + dir * gs_in[0].Velocity;
            gs_out.Velocity = gs_in[0].Velocity;
            gs_out.Age += age;
        }

        EmitVertex();
    }

    EndPrimitive(); 
}
