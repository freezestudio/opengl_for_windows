#version 330 core

in  vec3 Normal;
in  vec3 Pos;
in  vec2 UV;

out vec4 color;

//uniform vec3 light_position;
uniform vec3 view_position;
//uniform vec3 light_color;
//uniform vec3 cube_color;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess; //�߹�ɢ��\�뾶
};

uniform Material material;

struct Light{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

void main()
{
	//������
	vec3 ambient = light.ambient * texture(material.diffuse,UV).rgb;

	//������
    vec3 normal = normalize(Normal);
    vec3 light_direction = normalize(light.position - Pos);
    float diffuse_strength = max(dot(normal,light_direction),0.0f);
	vec3 diffuse = light.diffuse  * diffuse_strength * vec3(texture(material.diffuse,UV));

	//�����
    vec3 view_direction = normalize(view_position - Pos);
    vec3 reflect_direction = reflect(-light_direction,normal);

	float spec = pow(max(dot(view_direction,reflect_direction),0.0),material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular,UV));

	vec3  result  = (ambient + diffuse + specular);
    color = vec4(result,1.0f);
}
