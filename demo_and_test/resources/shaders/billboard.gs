#version 430 core

layout(points) in;                                                                  
layout(triangle_strip, max_vertices = 4) out;                                    
                                                                                   
uniform mat4 gVP;                                                                   
uniform vec3 gCameraPos;                                                            
uniform float gBillboardSize;                                                       
                                                                                    
out vec2 TexCoord;                                                                  
                                                                                    
void main()                                                                         
{                                                                                   
    vec3 Pos = gl_in[0].gl_Position.xyz;                                            
    vec3 toCamera = normalize(gCameraPos - Pos);                                    
    vec3 up = vec3(0.0f, 1.0f, 0.0f);                                                  
    vec3 right = cross(toCamera, up) * gBillboardSize;                              
                                                                                    
    Pos -= right;                                                                   
    gl_Position = gVP * vec4(Pos, 1.0f);                                             
    TexCoord = vec2(0.0f, 0.0f);                                                      
    EmitVertex();                                                                   
                                                                                    
    Pos.y += gBillboardSize;                                                        
    gl_Position = gVP * vec4(Pos, 1.0f);                                             
    TexCoord = vec2(0.0f, 1.0f);                                                      
    EmitVertex();                                                                   
                                                                                    
    Pos.y -= gBillboardSize;                                                        
    Pos += right;                                                                   
    gl_Position = gVP * vec4(Pos, 1.0f);                                             
    TexCoord = vec2(1.0f, 0.0f);                                                      
    EmitVertex();                                                                   
                                                                                    
    Pos.y += gBillboardSize;                                                        
    gl_Position = gVP * vec4(Pos, 1.0f);                                             
    TexCoord = vec2(1.0f, 1.0f);                                                      
    EmitVertex();                                                                   
                                                                                    
    EndPrimitive();                                                                 
}                                                                                   
