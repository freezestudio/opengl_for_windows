#version 330 core

in  vec3 Normal;
in  vec3 Pos;
out vec4 color;

uniform vec3 light_position;
uniform vec3 view_position;
uniform vec3 light_color;
uniform vec3 cube_color;

void main()
{
    float ambient_strength = 0.1f;
    vec3  ambient = ambient_strength * light_color;

    vec3 normal = normalize(Normal);
    vec3 light_direction = normalize(light_position - Pos);
    float diffuse_strength = max(dot(normal,light_direction),0.0f);
    vec3 diffuse = diffuse_strength * light_color;

    float specular_strength = 0.5f;
    vec3 view_direction = normalize(view_position - Pos);
    vec3 reflect_direction = reflect(-light_direction,normal);
    float spec = pow(max(dot(view_direction,reflect_direction),0.0f),32);
    vec3 specular = specular_strength * spec * light_color;

    vec3  result  = (ambient + diffuse + specular) * cube_color;
    color = vec4(result,1.0f);
}
