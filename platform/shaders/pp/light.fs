#version 330 core

in vec2 t_coords;

out vec4 o_color;

uniform sampler2D t_unit0; // color
uniform sampler2D t_unit1; // normal
uniform sampler2D t_unit2; // depth

uniform vec3 camera;
//uniform mat4 invViewProj;
uniform mat4 invProj;
uniform mat4 invView;
uniform float far;
uniform float near;

float ambientStrength = 0.3;
//vec3 lightColor = vec3(2, 2, 2);
vec3 lightColor = vec3(1, 1, 1);
//vec3 lightColor = vec3(0.4, 0.4, 0.4);
vec3 lightPos = vec3(0, 1, 1);

vec3 GetPosition(vec2 UV, float depth) {
    float z = depth * 2.0 - 1.0;
    vec4 clipSpacePosition = vec4(UV * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProj * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = invView * viewSpacePosition;
    return worldSpacePosition.xyz;
}

void main() {
	vec4 unit1 = texture(t_unit1, t_coords);
	
	// Vars
	vec3 color = texture(t_unit0, t_coords).rgb;
	vec3 normal = normalize(unit1.xyz*2 - 1);
	float spec = unit1.a;
	float depth = texture(t_unit2, t_coords).r;

	vec3 fragPos = GetPosition(t_coords, depth);

	// Lighting
	vec3 ambient = ambientStrength * lightColor;
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	vec3 viewDir = normalize(camera - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float s = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = spec * s * lightColor;
	
	vec3 result = (ambient + diffuse + specular) * color;

	o_color = vec4(result, 1);
	
	//o_color = vec4(color, 1);
}
