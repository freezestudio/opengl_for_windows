#version 430 core

uniform sampler2D gColorMap;                                                        
                                                                                    
in  vec2 TexCoord;                                                                   
out vec4 FragColor;                                                                 
                                                                                    
void main()                                                                         
{                                                                                   
    FragColor = texture(gColorMap, TexCoord);                                     
                                                                                    
    if (FragColor.r >= 0.9f && FragColor.g >= 0.9f && FragColor.b >= 0.9f)
    {           
        discard;                                                                    
    }                                                                               
}
