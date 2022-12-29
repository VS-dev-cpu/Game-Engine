#version 330 core

#define MAX_LIGHTS 8

struct Material
{
	bool lit;	   // is the fragment lit?
	bool textured; // is the fragment textured?

	// Transparencity
	float d;

	// Textures
	sampler2D diffuse;
	sampler2D specular;

	// Shininess
	float shininess;
};

struct Light
{
	int type; // -1 - disable, 0 - Directional, 1 - Point, 2 - Spot

	vec3 position;	// point, spot
	vec3 direction;	// dir, spot

	vec3 ambient;	// dir, point, spot
    vec3 diffuse;	// dir, point, spot
    vec3 specular;	// dir, point, spot

	float constant;		// point, spot
	float linear;		// point, spot
	float quadratic;	// point, spot

	float cutOff;		// spot
    float outerCutOff;	// spot
};

// function prototypes
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Light light[MAX_LIGHTS];
uniform Material mtl;

void main()
{
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	FragColor = texture(mtl.diffuse, TexCoord);

	if (mtl.lit)
	{
		vec3 result = vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			switch (light[i].type)
			{
			case 0: // Directional
				result += CalcDirLight(light[i], norm, viewDir);
				break;

			case 1: // Point
				result += CalcPointLight(light[i], norm, FragPos, viewDir);
				break;

			case 2: // Spot
				result += CalcSpotLight(light[i], norm, FragPos, viewDir);
				break;

			default:
				break;
			}
		}

		FragColor = vec4(result, mtl.d);
	}
}

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mtl.shininess);

    // combine results
    vec3 ambient = light.ambient * vec3(texture(mtl.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(mtl.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(mtl.specular, TexCoord));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mtl.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	// combine results
    vec3 ambient = light.ambient * vec3(texture(mtl.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(mtl.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(mtl.specular, TexCoord));
    
	ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mtl.shininess);
    
	// attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	// spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
	// combine results
    vec3 ambient = light.ambient * vec3(texture(mtl.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(mtl.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(mtl.specular, TexCoord));
    
	ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
	return (ambient + diffuse + specular);
}
