#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359f;

void main()
{		
    vec3 N = normalize(WorldPos);

    vec3 irradiance = vec3(0.0f);   
    
    // tangent space calculation from origin point
    vec3 up    = vec3(0.0f, 1.0f, 0.0f);
    vec3 right = cross(up, N);
    up            = cross(N, right);
       
    float sampleDelta = 0.025f;
    float nrSamples = 0.0f;
    for(float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
    {
        for(float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0f / float(nrSamples));
    
    FragColor = vec4(irradiance, 1.0f);
}
