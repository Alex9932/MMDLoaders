#version 330 core

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 normal;

in vec3 vs_position;
in vec3 vs_normal;
in vec3 vs_tangent;
in vec2 vs_uv;

uniform sampler2D t_unit0;

uniform vec4 d_color;
uniform vec3 a_color;
uniform vec3 s_color;
uniform float shininess;

vec4 pow4(vec4 vec, float i) {
	float x = pow(vec.x, i);
	float y = pow(vec.y, i);
	float z = pow(vec.z, i);
	float w = pow(vec.w, i);
	return vec4(x, y, z, w);
}

void main() {

	vec4 d_tex = texture(t_unit0, vs_uv);
	
	if(d_tex.a < 0.8) {
		discard;
	}

	color = vec4(a_color, 1) * d_tex;// * vec4(d_color.rgb, 1) * d_tex;
	//color = (vec4(a_color, 1) * d_tex) + (d_color * d_tex);
	//color.rgb = d_color.rgb;
	//color.rgb = vec3(s_color.r);
	//color.a = 1;
	
	//color.xyz *= 2.4;
	
	//color = pow4(vec4(a_color, 1) * d_tex, 0.9);
	
	normal = vec4(vs_normal*0.5 + 0.5, 1);
	
	
	normal.a = s_color.r;

}