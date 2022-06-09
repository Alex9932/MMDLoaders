#version 330 core

#define MAX_BONES 1024

layout (location = 0) in vec3 i_vertex;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec3 i_tangent;
layout (location = 3) in vec2 uv;

layout (location = 4) in vec4  i_bone_w;
layout (location = 5) in ivec4 i_bone_id;

out vec3 vs_position;
out vec3 vs_normal;
out vec3 vs_tangent;
out vec2 vs_uv;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

uniform int anim;

layout (std140) uniform BoneMatrices {
    mat4 bone_matrices[MAX_BONES];
};

void main() {

	vec4 total_position = vec4(0);
	vec3 total_normal   = vec3(0);
	vec3 total_tangent  = vec3(0);
	vec4 local_position = vec4(0);
	vec3 local_normal   = vec3(0);
	vec3 local_tangent  = vec3(0);
	
	// Skinning
	if(anim == 1) {
		for(int i = 0 ; i < 4; i++) {
			float weight = i_bone_w[i];
			int bone_id = i_bone_id[i];
		
			if(bone_id == -1) { continue; }
			if(bone_id >= MAX_BONES) {
				total_position = vec4(i_vertex, 1);
				total_normal = i_normal;
				break;
			}
			
			mat4 matrix = bone_matrices[bone_id];
			
			local_position =      matrix  * vec4(i_vertex, 1);
			local_normal   = mat3(matrix) * i_normal;
			total_position += (local_position * weight);
			total_normal   += (local_normal   * weight);
		}
	} else {
		total_position = vec4(i_vertex, 1);
		total_normal = i_normal;
	}

	vs_uv = uv;
	
	mat3 ti_model = mat3(transpose(inverse(model)));
	
	vs_normal  = normalize(ti_model * total_normal);
	vs_tangent = normalize(ti_model * total_tangent);
	//gl_Position = proj * view * model * total_position;
	
	vs_position = vec3(model * total_position);
	gl_Position = proj * view * vec4(vs_position, 1);

}