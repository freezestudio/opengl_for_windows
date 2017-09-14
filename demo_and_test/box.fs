#version 330 core

in  vec3 Normal;
in  vec3 Pos;
in  vec2 UV;

out vec4 color;

uniform vec3 view_position;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;//放射光贴图
	float shininess; //高光散射\半径
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
	//法线
    vec3 normal = normalize(Normal);

	//环境光
	vec3 ambient = light.ambient * texture(material.diffuse,UV).rgb;

	//漫反射
    vec3 light_direction = normalize(light.position - Pos);
    float diffuse_strength = max(dot(normal,light_direction),0.0f);
	vec3 diffuse = light.diffuse  * diffuse_strength * vec3(texture(material.diffuse,UV));

	//镜面光
    vec3 view_direction = normalize(view_position - Pos);
    vec3 reflect_direction = reflect(-light_direction,normal);
	float spec = pow(max(dot(view_direction,reflect_direction),0.0),material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular,UV));

	//放射光
	vec3 emission = texture(material.emission,UV).rgb;

	vec3  result  = (ambient + diffuse + specular) + emission;
    color = vec4(result,1.0f);
}
