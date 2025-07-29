#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Material PBR properties
uniform sampler2D albedoMap;
uniform vec4  albedoFactor;
uniform float metallicFactor;
uniform float roughnessFactor;

// Lights
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

const float PI = 3.14159265359;

// PBR helper functions
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = NdotV / (NdotV * (1.0 - k) + k);
    float ggx1  = NdotL / (NdotL * (1.0 - k) + k);
	
    return ggx1 * ggx2;
}


void main()
{		
    vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2)) * albedoFactor.rgb;
    float metallic  = metallicFactor;
    float roughness = roughnessFactor;

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // Reflectance equation
    vec3 Lo = vec3(0.0);
    
    // For now, a single point light
    vec3 L = normalize(lightPos - FragPos);
    vec3 H = normalize(V + L);
    float distance    = length(lightPos - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = lightColor * attenuation;
        
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; 
    vec3 specular     = numerator / denominator;
        
    float NdotL = max(dot(N, L), 0.0);        
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.03) * albedo;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    FragColor = vec4(color, 1.0);
}
