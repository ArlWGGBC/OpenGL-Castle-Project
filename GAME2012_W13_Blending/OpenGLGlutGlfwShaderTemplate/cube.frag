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

struct Material
{
	float specularStrength;
	float shininess;
};

uniform sampler2D texture0;
uniform AmbientLight aLight;
uniform Material mat;

vec4 calcAmbientLight(Light a)
{
	vec4 ambient = vec4(a.diffuseColour, 1.0f) * a.diffuseStrength;
	return ambient;
}

void main()
{
	// Calculate lighting.
	vec4 calcColor = vec4(0.0, 0.0, 0.0, 1.0);
	calcColor += calcAmbientLight(aLight.base);
		
	frag_color = texture(texture0, texCoord) * vec4(color); //* calcColor;
}