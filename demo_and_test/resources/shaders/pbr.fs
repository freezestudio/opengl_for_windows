#version 330 core

out vec4 FragColor;
in VS
{
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
} fs_in;

struct Material
{
    sampler2D albedo;
    sampler2D normal;
    sampler2D orm;
};

uniform Material material;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;


// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

const float PI = 3.14159265359f;

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
vec3 getNormalFromMap(vec3 tN)
{
    vec3 Q1  = dFdx(fs_in.WorldPos);
    vec3 Q2  = dFdy(fs_in.WorldPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tN);
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{    
	float a = pow(roughness,2.0f);
	float NdotH = max(dot(N,H),0.0f);
	return pow(a,2.0f)/(PI* pow(pow(NdotH,2.0f)*(pow(a,2.0f)-1) + 1,2.0f));
}

// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float a = (roughness + 1.0f);
    float k = (a*a) / 8.0f;

    float nom   = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    //return F0 + (1.0f - F0) * pow(1.0f- cosTheta, 5.0f);
	return F0 +(1.0f-F0)*pow(2.0f,(-5.55473f*cosTheta-6.98316f)*cosTheta);
}

// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    //return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
	return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(2.0f,(-5.55473f*cosTheta-6.98316f)*cosTheta);
}   

// ----------------------------------------------------------------------------
void main()
{		
    vec3  albedo    = pow(texture(material.albedo, fs_in.TexCoords).rgb, vec3(2.2f));
	vec3  orm       = texture(material.orm, fs_in.TexCoords).rgb;
    float metallic  = orm.b;
    float roughness = orm.g;
    float ao        = orm.r;
		
    vec3 tangentNormal = texture(material.normal, fs_in.TexCoords).xyz * 2.0f - 1.0f;
    vec3 N = getNormalFromMap(tangentNormal);
    vec3 V = normalize(camPos - fs_in.WorldPos);
    vec3 R = reflect(-V,N);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04f); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0f);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - fs_in.WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - fs_in.WorldPos);
        float attenuation = 1.0f / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3  F   = fresnelSchlick(max(dot(N, V), 0.0f), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.001f; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0f) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0f - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0f);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    //vec3 ambient = vec3(0.03f) * albedo * ao;

    vec3 F = fresnelSchlickRoughness(max(dot(N,V),0.0f),F0,roughness);

    vec3 kS = F;
    vec3 kD = vec3(1.0f) - kS;
    kD *= 1.0f - metallic;

    vec3 irradiance = texture(irradianceMap,N).rgb;
    vec3 diffuse    = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0f;
    vec3 prefilteredColor = textureLod(prefilterMap,R,roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT,vec2(max(dot(N,V),0.0f),roughness)).rg;
    vec3 specular = prefilteredColor * (F*brdf.x+brdf.y);

    vec3 ambient = (kD*diffuse + specular) * ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0f));
    // gamma correct
    color = pow(color, vec3(1.0f/2.2f)); 

    FragColor = vec4(color, 1.0f);
}