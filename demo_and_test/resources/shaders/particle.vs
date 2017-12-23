#version 430 core
                                               
layout (location = 0) in vec3  Position;                                             
layout (location = 1) in vec3  Velocity;                                             
layout (location = 2) in float Age;                                                
     
out VS
{
    vec3  Position;                                                                 
    vec3  Velocity;                                                                 
    float Age;
} vs_out;                                                          
                                                                                    
void main()                                                                         
{                                                                   
    vs_out.Position = Position;                                                           
    vs_out.Velocity = Velocity;                                                           
    vs_out.Age      = Age;                                                                      
}
