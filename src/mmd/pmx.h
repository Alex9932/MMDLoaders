/*
 * pmx.h
 *
 *  Created on: Jan 25, 2022
 *      Author: alex9932
 *
 *  PMX (Polygon Model eXtended) Implementation
 *  Spec:
 *  	https://gist.github.com/felixjones/f8a06bd48f9da9a4539f
 *  	https://github.com/ephemeral-laboratories/modelling_formats/blob/main/mmd_pmx.ksy
 */

#ifndef PMX_H_
#define PMX_H_

#include <SDL2/SDL.h>

// Material flags
#define PMX_MATFLAG_NOCULL        0x1
#define PMX_MATFLAG_GROUND_SHADOW 0x2
#define PMX_MATFLAG_DRAW_SHADOW   0x4
#define PMX_MATFLAG_RECV_SHADOW   0x8
#define PMX_MATFLAG_HAS_EDGE      0x10
#define PMX_MATFLAG_VERTEX_COLOR  0x20 // PMX 2.1
#define PMX_MATFLAG_POINT_DRAWING 0x40 // PMX 2.1
#define PMX_MATFLAG_LINE_DRAWING  0x80 // PMX 2.1

// Bone flags
#define PMX_BONEFLAG_INDEXED_TAIL           0x1
#define PMX_BONEFLAG_ROTATABLE              0x2
#define PMX_BONEFLAG_TRANSLATABLE           0x4
#define PMX_BONEFLAG_IS_VISIBLE             0x8
#define PMX_BONEFLAG_ENABLED                0x10
#define PMX_BONEFLAG_IK                     0x20
#define PMX_BONEFLAG_INHERIT_ROTATION       0x100
#define PMX_BONEFLAG_INHERIT_TRANSLATION    0x200
#define PMX_BONEFLAG_FIXED_AXIS             0x400
#define PMX_BONEFLAG_LOCAL_COORDINATE       0x800
#define PMX_BONEFLAG_PHYSICS_AFTER_DEFORM   0x1000
#define PMX_BONEFLAG_EXTERNAL_PARENT_DEFORM 0x2000

// Morph types
#define PMX_MORPH_GROUP    0x00
#define PMX_MORPH_VERTEX   0x01
#define PMX_MORPH_BONE     0x02
#define PMX_MORPH_UV       0x03
#define PMX_MORPH_UV1      0x04
#define PMX_MORPH_UV2      0x05
#define PMX_MORPH_UV3      0x06
#define PMX_MORPH_UV4      0x07
#define PMX_MORPH_MATERIAL 0x08
#define PMX_MORPH_FLIP     0x09
#define PMX_MORPH_IMPULSE  0x0A

// PMX Types
typedef Uint8 pmx_flag;

typedef struct pmx_vec2 {
	float x;
	float y;
} pmx_vec2;

typedef struct pmx_vec3 {
	float x;
	float y;
	float z;
} pmx_vec3;

typedef struct pmx_vec4 {
	float x;
	float y;
	float z;
	float w;
} pmx_vec4;

typedef struct pmx_text {
	Sint32 len;
	char* data;
} pmx_text;

// PMX FILE

typedef struct pmx_header {
	Uint8 signature[4];
	float version;
	Uint8 globals_count;
	Uint8 g_text_encoding;
	Uint8 g_additional_vec4_count;
	Uint8 g_vertex_index_size;
	Uint8 g_texture_index_size;
	Uint8 g_material_index_size;
	Uint8 g_bone_index_size;
	Uint8 g_morph_index_size;
	Uint8 g_rigidbody_index_size;
	pmx_text model_name;
	pmx_text model_name_en;
	pmx_text model_comment;
	pmx_text model_comment_en;
} pmx_header;

typedef struct pmx_weight {
	float weights[4];
	Sint32 bone_id[4];
} pmx_weight;

typedef struct pmx_vertex {
	pmx_vec3 position;
	pmx_vec3 normal;
	pmx_vec2 uv;
	pmx_vec4* additional_vec4; // NULL (NOT IMPLEMENTED YET)
	Uint8 type;
	pmx_weight weight;
	float edge_scale;
} pmx_vertex;

typedef struct pmx_texture {
	pmx_text path;
} pmx_texture;

typedef struct pmx_material {
	pmx_text name;
	pmx_text name_en;
	pmx_text comment;
	pmx_vec4 diffuse_color;
	pmx_vec3 specular_color;
	float specular_strength; // shininess
	pmx_vec3 ambient_color;
	pmx_flag drawing_flags;
	pmx_vec4 edge_color;
	float edge_scale;
	Uint32 texture_id;
	Uint32 texture_id_env;
	Uint8 env_blend_mode;
	Uint8 toon_reference;
	Uint32 toon_value;
	Uint32 surface_count;
} pmx_material;

typedef struct pmx_ik_link {
	Sint32 bone_index;
	Sint8 has_limits;
	pmx_vec3 min;
	pmx_vec3 max;
} pmx_ik_link;

typedef struct pmx_bone {
	pmx_text name;
	pmx_text name_en;
	pmx_vec3 position;
	Sint32 parent_id;
	Sint32 layer;
	Uint16 flags;
	// connect_index OR tail_pos (check PMX_BONEFLAG_INDEXED_TAIL flag)
	Sint32 connect_index;
	pmx_vec3 tail_pos;
	// check PMX_BONEFLAG_INHERIT_ROTATION || PMX_BONEFLAG_INHERIT_TRANSLATION flag
	Sint32 inh_index;
	float inh_influence;
	// check PMX_BONEFLAG_FIXED_AXIS flag
	pmx_vec3 fixed_axis;
	// check PMX_BONEFLAG_LOCAL_COORDINATE flag
	pmx_vec3 local_x;
	pmx_vec3 local_z;
	// check PMX_BONEFLAG_EXTERNAL_PARENT_DEFORM flag
	Uint32 key;
	// check PMX_BONEFLAG_IK flag
	Sint32 ik_target_index;
	Sint32 ik_loop_count;
	float ik_limit_radian;
	Sint32 ik_link_count;
	pmx_ik_link* ik_links;
} pmx_bone;

typedef struct pmx_morph_offset {
	// Group
	Uint32 morph_index;
	float morph_weight;
	// Vertex
	Uint32 vertex_index;
	pmx_vec3 vertex_pos;
	// Bone
	Uint32 bone_index;
	pmx_vec3 bone_pos;
	pmx_vec4 bone_rot;
	// UV
	Uint32 uv_index;
	pmx_vec4 uv_floats;
	// Material
	Uint32 material_index;
	pmx_vec4 material_diffuse;
	pmx_vec3 material_specular;
	float material_shininess;
	pmx_vec3 material_ambient;
	pmx_vec4 material_edgecolor;
	float material_edgesize;
	pmx_vec4 material_texture_tint;
	pmx_vec4 material_env_tint;
	pmx_vec4 material_toon_tint;
	// Flip
	Uint32 flip_index;
	float flip_weight;
	// Impulse
	Uint32 imp_index;
	Uint32 imp_local_flag;
	pmx_vec3 imp_speed;
	pmx_vec3 imp_torque;
} pmx_morph_offset;

typedef struct pmx_morph {
	pmx_text name;
	pmx_text name_en;
	Uint16 panel_type;
	Uint16 morph_type;
	Uint32 offset_count;
	pmx_morph_offset* offsets;
} pmx_morph;

typedef struct pmx_file {
	pmx_header header;
	Sint32 vertex_count;
	Sint32 index_count;
	Sint32 texture_count;
	Sint32 material_count;
	Sint32 bone_count;
	Sint32 morph_count;
	pmx_vertex* vertices;
	Uint16* indices;
	pmx_texture* textures;
	pmx_material* materials;
	pmx_bone* bones;
	pmx_morph* morphs;
} pmx_file;

pmx_file* pmx_load(const char* path);
void pmx_free(pmx_file* ptr);

#endif /* PMX_H_ */
