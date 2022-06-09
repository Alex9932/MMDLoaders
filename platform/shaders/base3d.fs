#version 330 core

layout (location = 0) out vec4 color;

in vec3 vs_position;
in vec3 vs_normal;
in vec3 vs_tangent;
in vec2 vs_uv;

uniform sampler2D t_unit0;

uniform vec4 d_color;
uniform vec3 a_color;
uniform vec3 s_color;

vec4 pow4(vec4 vec, float i) {
	float x = pow(vec.x, i);
	float y = pow(vec.y, i);
	float z = pow(vec.z, i);
	float w = pow(vec.w, i);
	return vec4(x, y, z, w);
}

void main() {

	vec4 d_tex = texture(t_unit0, vs_uv);

	color = vec4(a_color, 1) * d_tex;// * vec4(d_color.rgb, 1) * d_tex;
	
	color.xyz *= 2;
	
	//color = pow4(vec4(a_color, 1) * d_tex, 0.9);
	
	//color = vec4(vs_normal*0.5 + 0.5, 1);

}