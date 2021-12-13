#version 430 core

in vec4 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 frag_color;

struct Light
{
	vec3 diffuseColour;
	float diffuseStrength;
};

struct AmbientLight 
{
	Light base;
};

struct DirectionalLight 
{
	Light base;
	vec3 origin;
};

struct PointLight
{
	Light base;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};



struct Material
{
	float specularStrength;
	float shininess;
};

uniform sampler2D texture0;
uniform vec3 eyePosition;

uniform AmbientLight aLight;
uniform DirectionalLight dLight;
uniform PointLight pLight;
uniform PointLight pLight2;
uniform PointLight pLight3;
uniform Material mat;

vec4 calcAmbientLight(Light a)
{
	vec4 ambient = vec4(a.diffuseColour, 1.0f) * a.diffuseStrength;
	return ambient;
}
vec4 calcLightByDirection(Light l, vec3 dir)
{
	float diffuseFactor = max( dot( normalize(normal), normalize(dir) ), 0.0f);
	vec4 diffuse = vec4(l.diffuseColour, 1.0f) * l.diffuseStrength * diffuseFactor;

	vec4 specular = vec4(0,0,0,0);
	if (diffuseFactor > 0.0f && l.diffuseStrength > 0.0f)
	{
		vec3 fragToEye = normalize(eyePosition - fragPos);
		vec3 reflectedVertex = normalize(reflect(dir, normalize(normal)));

		float specularFactor = dot(fragToEye, reflectedVertex);
		if (specularFactor > 0.0f)
		{
			specularFactor = pow(specularFactor, mat.shininess);
			specular = vec4(l.diffuseColour * mat.specularStrength * specularFactor, 1.0f);
		}
	}
	return (diffuse + specular);
}

vec4 calcDirectionalLight()
{
	return calcLightByDirection(dLight.base, dLight.origin);
}

vec4 calcPointLight(PointLight p)
{
	vec3 direction = p.position - fragPos;
	float distance = length(direction);
	direction = normalize(direction);
		
	vec4 color = calcLightByDirection(p.base, direction);
	float attenuation = 1 / (p.quadratic * distance * distance +
						p.linear * distance +
						p.constant);
	
	return (color * attenuation);
}



void main()
{
	// Calculate lighting.
	vec4 calcColor = vec4(0.0, 0.0, 0.0, 1.0);
	calcColor += calcAmbientLight(aLight.base);
	calcColor += calcDirectionalLight();
	calcColor += calcPointLight(pLight);
	calcColor += calcPointLight(pLight2);
	calcColor += calcPointLight(pLight3);

		
	frag_color = texture(texture0, texCoord) * vec4(color) * calcColor;
}