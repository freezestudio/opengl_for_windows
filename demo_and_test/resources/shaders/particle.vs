#version 430 core
                                               
layout (location = 0) in vec3  Position;                                             
layout (location = 1) in vec3  Velocity;                                             
layout (location = 2) in float Age;
layout (location = 3) in float Type;                                                  
                                                                    
out vec3  Position0;                                                                 
out vec3  Velocity0;                                                                 
out float Age0;                                                                                  
out float Type0;                                                                       
                                                                                    
void main()                                                                         
{                                                                   
    Position0 = Position;                                                           
    Velocity0 = Velocity;                                                           
    Age0      = Age;                                                                                
    Type0     = Type;                                                                        
}
