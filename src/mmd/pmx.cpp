/*
 * pmx.cpp
 *
 *  Created on: Jan 27, 2022
 *      Author: alex9932
 */

#include "pmx.h"
#include <stdio.h>
#include <stdlib.h>

#include <engine/engine.h>

#define PMX_DEBUG

typedef struct __attribute__((packed)) _pmx_header {
	Uint8 signature[4];            // (PMX [0x50, 0x4D, 0x58, 0x20])
	float version;                 // 2.0 / 2.1
	Uint8 globals_count;           // For pmx v2.0 fixed at 8
	// This implementation supports only 8 globals
	Uint8 g_text_encoding;         // Text encoding 0 = UTF16LE, 1 = UTF8
	Uint8 g_additional_vec4_count; // Additional vec4 values are added to each vertex (0 .. 4)
	Uint8 g_vertex_index_size;     // Index types {1, 2 or 4}
	Uint8 g_texture_index_size;
	Uint8 g_material_index_size;
	Uint8 g_bone_index_size;
	Uint8 g_morph_index_size;
	Uint8 g_rigidbody_index_size;
} _pmx_header;

// Temporary implementation
typedef struct __attribute__((packed)) _pmx_vertex {
	pmx_vec3 position;
	pmx_vec3 normal;
	pmx_vec2 uv;
} _pmx_vertex;

typedef struct __attribute__((packed)) _pmx_material {
	pmx_vec4 diffuse_color;
	pmx_vec3 specular_color;
	float specular_strength;
	pmx_vec3 ambient_color;
	pmx_flag drawing_flags;
	pmx_vec4 edge_color;
	float edge_scale;
} _pmx_material;

static Sint32 _pmx_readSINT(Uint8 size, FILE* file) {
	Sint32 index = -1;
	switch (size) {
		case 1:
			Sint8 id1;
			fread(&id1, 1, 1, file);
			index = id1;
			break;
		case 2:
			Sint16 id2;
			fread(&id2, 2, 1, file);
			index = id2;
			break;
		case 4:
			Sint32 id3;
			fread(&id3, 4, 1, file);
			index = id3;
			break;
		default:
			break;
	}
	return index;
}

static char string_buffer[128];

static void _pmx_readVertex(Sint32 id, pmx_file* pmx, FILE* file) {
	pmx_vertex vertex;
	memset(&vertex, 0, sizeof(pmx_vertex));

	_pmx_vertex vert;
	fread(&vert, sizeof(_pmx_vertex), 1, file);
	vertex.position = vert.position;
	vertex.normal = vert.normal;
	vertex.uv = vert.uv;

	// Additional vec4
	vertex.additional_vec4 = (pmx_vec4*)malloc(sizeof(pmx_vec4) * pmx->header.g_additional_vec4_count);
	fread(vertex.additional_vec4, sizeof(pmx_vec4), pmx->header.g_additional_vec4_count, file);

	fread(&vertex.type, sizeof(Uint8), 1, file);

	// Weights
	switch (vertex.type) {
		case 0: // BDEF 1
			vertex.weight.bone_id[0] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[1] = -1;
			vertex.weight.bone_id[2] = -1;
			vertex.weight.bone_id[3] = -1;
			vertex.weight.weights[0] = 1;
			vertex.weight.weights[1] = 0;
			vertex.weight.weights[2] = 0;
			vertex.weight.weights[3] = 0;
			break;
		case 1: // BDEF 2
			vertex.weight.bone_id[0] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[1] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[2] = -1;
			vertex.weight.bone_id[3] = -1;
			fread(&vertex.weight.weights[0], sizeof(float), 1, file);
			vertex.weight.weights[1] = 1.0f - vertex.weight.weights[0];
			vertex.weight.weights[2] = 0;
			vertex.weight.weights[3] = 0;
			break;
		case 2: // BDEF 4
			vertex.weight.bone_id[0] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[1] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[2] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[3] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			fread(&vertex.weight.weights[0], sizeof(float), 1, file);
			fread(&vertex.weight.weights[1], sizeof(float), 1, file);
			fread(&vertex.weight.weights[2], sizeof(float), 1, file);
			fread(&vertex.weight.weights[3], sizeof(float), 1, file);
			break;
		case 3: // SDEF
			vertex.weight.bone_id[0] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[1] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[2] = -1;
			vertex.weight.bone_id[3] = -1;
			fread(&vertex.weight.weights[0], sizeof(float), 1, file);
			vertex.weight.weights[1] = 1.0f - vertex.weight.weights[0];
			vertex.weight.weights[2] = 0;
			vertex.weight.weights[3] = 0;
			pmx_vec3 C, R0, R1; // Not supported! Just read 3 vec3
			fread(&C, sizeof(pmx_vec3), 1, file);
			fread(&R0, sizeof(pmx_vec3), 1, file);
			fread(&R1, sizeof(pmx_vec3), 1, file);
			break;
		case 4: // QDEF
			vertex.weight.bone_id[0] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[1] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[2] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			vertex.weight.bone_id[3] = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			fread(&vertex.weight.weights[0], sizeof(float), 1, file);
			fread(&vertex.weight.weights[1], sizeof(float), 1, file);
			fread(&vertex.weight.weights[2], sizeof(float), 1, file);
			fread(&vertex.weight.weights[3], sizeof(float), 1, file);
			break;
		default:
			break;
	}

	fread(&vertex.edge_scale, sizeof(float), 1, file);
	pmx->vertices[id] = vertex;
}

static pmx_text _pmx_readText(FILE* file) {
	pmx_text text;
	fread(&text.len, sizeof(Sint32), 1, file);
	text.data = (char*)malloc(text.len);
	fread(text.data, 1, text.len, file);
	return text;
}

static void _pmx_readTexture(Sint32 id, pmx_file* pmx, FILE* file) {
	pmx_texture texture;
	texture.path = _pmx_readText(file);
	pmx->textures[id] = texture;
}

static void _pmx_readMaterial(Sint32 id, pmx_file* pmx, FILE* file) {
	pmx_material material;
	memset(&material, 0, sizeof(pmx_material));

	material.name = _pmx_readText(file);
	material.name_en = _pmx_readText(file);

	_pmx_material mat;
	fread(&mat, sizeof(_pmx_material), 1, file);

	material.diffuse_color = mat.diffuse_color;
	material.specular_color = mat.specular_color;
	material.specular_strength = mat.specular_strength;
	material.ambient_color = mat.ambient_color;
	material.drawing_flags = mat.drawing_flags;
	material.edge_color = mat.edge_color;
	material.edge_scale = mat.edge_scale;

	fread(&material.texture_id, pmx->header.g_texture_index_size, 1, file);
	fread(&material.texture_id_env, pmx->header.g_texture_index_size, 1, file);
	fread(&material.env_blend_mode, sizeof(Uint8), 1, file);
	fread(&material.toon_reference, sizeof(Uint8), 1, file);
	if(material.toon_reference) {
		fread(&material.toon_value, sizeof(Uint8), 1, file);
	} else {
		fread(&material.toon_value, pmx->header.g_texture_index_size, 1, file);
	}

	material.comment = _pmx_readText(file);
	fread(&material.surface_count, sizeof(Sint32), 1, file);

	pmx->materials[id] = material;
}

static void _pmx_readBone(Sint32 id, pmx_file* pmx, FILE* file) {
	pmx_bone bone;

	bone.name = _pmx_readText(file);
	bone.name_en = _pmx_readText(file);
	fread(&bone.position, sizeof(pmx_vec3), 1, file);
	bone.parent_id = _pmx_readSINT(pmx->header.g_bone_index_size, file);
	fread(&bone.layer, sizeof(Sint32), 1, file);
	fread(&bone.flags, sizeof(Uint16), 1, file);

	if(bone.flags & PMX_BONEFLAG_INDEXED_TAIL) { // Offset position
		bone.connect_index = _pmx_readSINT(pmx->header.g_bone_index_size, file);
	} else {
		fread(&bone.tail_pos, sizeof(pmx_vec3), 1, file);
	}

	// Inherit bone
	if((bone.flags & PMX_BONEFLAG_INHERIT_ROTATION) || (bone.flags & PMX_BONEFLAG_INHERIT_TRANSLATION)) {
		bone.inh_index = _pmx_readSINT(pmx->header.g_bone_index_size, file);
		fread(&bone.inh_influence, sizeof(float), 1, file);
	}

	// Fixed axis
	if(bone.flags & PMX_BONEFLAG_FIXED_AXIS) {
		fread(&bone.fixed_axis, sizeof(pmx_vec3), 1, file);
	}

	// Loacal co-ordinate
	if(bone.flags & PMX_BONEFLAG_LOCAL_COORDINATE) {
		fread(&bone.local_x, sizeof(pmx_vec3), 1, file);
		fread(&bone.local_z, sizeof(pmx_vec3), 1, file);
	}

	// External parent
	if(bone.flags & PMX_BONEFLAG_EXTERNAL_PARENT_DEFORM) {
		fread(&bone.key, sizeof(Uint32), 1, file);
//		readSINT(header.g_bone_index_size, file);
	}

	// IK
	if(bone.flags & PMX_BONEFLAG_IK) {
		bone.ik_target_index = _pmx_readSINT(pmx->header.g_bone_index_size, file);
		fread(&bone.ik_loop_count, sizeof(Sint32), 1, file);
		fread(&bone.ik_limit_radian, sizeof(float), 1, file);
		fread(&bone.ik_link_count, sizeof(Sint32), 1, file);
		bone.ik_links = (pmx_ik_link*)malloc(sizeof(pmx_ik_link) * bone.ik_link_count);

		//IK Links
		for (Sint32 i = 0; i < bone.ik_link_count; ++i) {

			bone.ik_links[i].bone_index = _pmx_readSINT(pmx->header.g_bone_index_size, file);
			fread(&bone.ik_links[i].has_limits, sizeof(Sint8), 1, file);
			if(bone.ik_links[i].has_limits) {
				// IK Angle limits
				fread(&bone.ik_links[i].min, sizeof(pmx_vec3), 1, file);
				fread(&bone.ik_links[i].max, sizeof(pmx_vec3), 1, file);
			}
		}
	}

	pmx->bones[id] = bone;
}

static void _pmx_readMorphGroup(Sint32 id, pmx_file* pmx, pmx_morph* morph, FILE* file) {
	if(pmx->header.g_morph_index_size == 1) {
		Uint8 u8;
		fread(&u8, 1, 1, file);
		morph->offsets[id].morph_index = u8;
	} else if(pmx->header.g_morph_index_size == 2) {
		Uint16 u16;
		fread(&u16, 2, 1, file);
		morph->offsets[id].morph_index = u16;
	} else if(pmx->header.g_morph_index_size == 4) {
		fread(&morph->offsets[id].morph_index, 4, 1, file);
	}
	fread(&morph->offsets[id].morph_weight, sizeof(float), 1, file);
}

static void _pmx_readMorphVertex(Sint32 id, pmx_file* pmx, pmx_morph* morph, FILE* file) {
	if(pmx->header.g_vertex_index_size == 1) {
		Uint8 u8;
		fread(&u8, 1, 1, file);
		morph->offsets[id].vertex_index = u8;
	} else if(pmx->header.g_vertex_index_size == 2) {
		Uint16 u16;
		fread(&u16, 2, 1, file);
		morph->offsets[id].vertex_index = u16;
	} else if(pmx->header.g_vertex_index_size == 4) {
		fread(&morph->offsets[id].vertex_index, 4, 1, file);
	}
	fread(&morph->offsets[id].vertex_pos, sizeof(pmx_vec3), 1, file);
}

static void _pmx_readMorphBone(Sint32 id, pmx_file* pmx, pmx_morph* morph, FILE* file) {
	if(pmx->header.g_bone_index_size == 1) {
		Uint8 u8;
		fread(&u8, 1, 1, file);
		morph->offsets[id].bone_index = u8;
	} else if(pmx->header.g_bone_index_size == 2) {
		Uint16 u16;
		fread(&u16, 2, 1, file);
		morph->offsets[id].bone_index = u16;
	} else if(pmx->header.g_bone_index_size == 4) {
		fread(&morph->offsets[id].bone_index, 4, 1, file);
	}
	fread(&morph->offsets[id].bone_pos, sizeof(pmx_vec3), 1, file);
	fread(&morph->offsets[id].bone_rot, sizeof(pmx_vec4), 1, file);
}

static void _pmx_readMorphUV(Sint32 id, pmx_file* pmx, pmx_morph* morph, FILE* file) {
	if(pmx->header.g_vertex_index_size == 1) {
		Uint8 u8;
		fread(&u8, 1, 1, file);
		morph->offsets[id].uv_index = u8;
	} else if(pmx->header.g_vertex_index_size == 2) {
		Uint16 u16;
		fread(&u16, 2, 1, file);
		morph->offsets[id].uv_index = u16;
	} else if(pmx->header.g_vertex_index_size == 4) {
		fread(&morph->offsets[id].uv_index, 4, 1, file);
	}
	fread(&morph->offsets[id].uv_floats, sizeof(pmx_vec4), 1, file);
}

static void _pmx_readMorphMaterial(Sint32 id, pmx_file* pmx, pmx_morph* morph, FILE* file) {
	if(pmx->header.g_material_index_size == 1) {
		Uint8 u8;
		fread(&u8, 1, 1, file);
		morph->offsets[id].material_index = u8;
	} else if(pmx->header.g_material_index_size == 2) {
		Uint16 u16;
		fread(&u16, 2, 1, file);
		morph->offsets[id].material_index = u16;
	} else if(pmx->header.g_material_index_size == 4) {
		fread(&morph->offsets[id].material_index, 4, 1, file);
	}

	Uint8 u8;
	fread(&u8, 1, 1, file); // Unknown byte
	fread(&morph->offsets[id].material_diffuse, sizeof(pmx_vec4), 1, file);
	fread(&morph->offsets[id].material_specular, sizeof(pmx_vec3), 1, file);
	fread(&morph->offsets[id].material_shininess, sizeof(float), 1, file);
	fread(&morph->offsets[id].material_ambient, sizeof(pmx_vec3), 1, file);
	fread(&morph->offsets[id].material_edgecolor, sizeof(pmx_vec4), 1, file);
	fread(&morph->offsets[id].material_edgesize, sizeof(float), 1, file);
	fread(&morph->offsets[id].material_texture_tint, sizeof(pmx_vec4), 1, file);
	fread(&morph->offsets[id].material_env_tint, sizeof(pmx_vec4), 1, file);
	fread(&morph->offsets[id].material_toon_tint, sizeof(pmx_vec4), 1, file);
}

static void _pmx_readMorphFlip(Sint32 id, pmx_file* pmx, pmx_morph* morph, FILE* file) {
	if(pmx->header.g_morph_index_size == 1) {
		Uint8 u8;
		fread(&u8, 1, 1, file);
		morph->offsets[id].flip_index = u8;
	} else if(pmx->header.g_morph_index_size == 2) {
		Uint16 u16;
		fread(&u16, 2, 1, file);
		morph->offsets[id].flip_index = u16;
	} else if(pmx->header.g_morph_index_size == 4) {
		fread(&morph->offsets[id].flip_index, 4, 1, file);
	}
	fread(&morph->offsets[id].flip_weight, sizeof(float), 1, file);
}

static void _pmx_readMorphImpulse(Sint32 id, pmx_file* pmx, pmx_morph* morph, FILE* file) {
	if(pmx->header.g_rigidbody_index_size == 1) {
		Uint8 u8;
		fread(&u8, 1, 1, file);
		morph->offsets[id].imp_index = u8;
	} else if(pmx->header.g_rigidbody_index_size == 2) {
		Uint16 u16;
		fread(&u16, 2, 1, file);
		morph->offsets[id].imp_index = u16;
	} else if(pmx->header.g_rigidbody_index_size == 4) {
		fread(&morph->offsets[id].imp_index, 4, 1, file);
	}

	Uint8 u8;
	fread(&u8, 1, 1, file);
	morph->offsets[id].imp_local_flag = u8;
	fread(&morph->offsets[id].imp_speed, sizeof(pmx_vec3), 1, file);
	fread(&morph->offsets[id].imp_torque, sizeof(pmx_vec3), 1, file);
}

static void _pmx_readMorph(Sint32 id, pmx_file* pmx, FILE* file) {
	pmx_morph morph;

	morph.name = _pmx_readText(file);
	morph.name_en = _pmx_readText(file);

	Uint8 u8;
	fread(&u8, 1, 1, file);
	morph.panel_type = u8;
	fread(&u8, 1, 1, file);
	morph.morph_type = u8;
	fread(&morph.offset_count, sizeof(Uint32), 1, file);

	morph.offsets = (pmx_morph_offset*)malloc(sizeof(pmx_morph_offset) * morph.offset_count);
	for (Uint32 i = 0; i < morph.offset_count; ++i) {
		switch (morph.morph_type) {
			case PMX_MORPH_GROUP:
				_pmx_readMorphGroup(i, pmx, &morph, file);
				break;
			case PMX_MORPH_VERTEX:
				_pmx_readMorphVertex(i, pmx, &morph, file);
				break;
			case PMX_MORPH_BONE:
				_pmx_readMorphBone(i, pmx, &morph, file);
				break;
			case PMX_MORPH_UV:
			case PMX_MORPH_UV1:
			case PMX_MORPH_UV2:
			case PMX_MORPH_UV3:
			case PMX_MORPH_UV4:
				_pmx_readMorphUV(i, pmx, &morph, file);
				break;
			case PMX_MORPH_MATERIAL:
				_pmx_readMorphMaterial(i, pmx, &morph, file);
				break;
			case PMX_MORPH_FLIP:
				_pmx_readMorphFlip(i, pmx, &morph, file);
				break;
			case PMX_MORPH_IMPULSE:
				_pmx_readMorphImpulse(i, pmx, &morph, file);
				break;
			default:
				break;
		}
	}

	pmx->morphs[id] = morph;
}

pmx_file* pmx_load(const char* path) {
	FILE* file = fopen(path, "rb");

	sprintf(string_buffer, "FILE NOT FOUND => %s", path);
	RG_ASSERT_MSG(file, string_buffer);

	printf("PMX: Loading model: %s\n", path);

	pmx_file* pmxFile = (pmx_file*)malloc(sizeof(pmx_file));

	// Read header
	_pmx_header header;
	fread(&header, sizeof(_pmx_header), 1, file);
	pmxFile->header.signature[0] = header.signature[0];
	pmxFile->header.signature[1] = header.signature[1];
	pmxFile->header.signature[2] = header.signature[2];
	pmxFile->header.signature[3] = header.signature[3];
	pmxFile->header.version = header.version;
	pmxFile->header.globals_count = header.globals_count;
	pmxFile->header.g_text_encoding = header.g_text_encoding;
	pmxFile->header.g_additional_vec4_count = header.g_additional_vec4_count;
	pmxFile->header.g_vertex_index_size = header.g_vertex_index_size;
	pmxFile->header.g_texture_index_size = header.g_texture_index_size;
	pmxFile->header.g_material_index_size = header.g_material_index_size;
	pmxFile->header.g_bone_index_size = header.g_bone_index_size;
	pmxFile->header.g_morph_index_size = header.g_morph_index_size;
	pmxFile->header.g_rigidbody_index_size = header.g_rigidbody_index_size;
	pmxFile->header.model_name = _pmx_readText(file);
	pmxFile->header.model_name_en = _pmx_readText(file);
	pmxFile->header.model_comment = _pmx_readText(file);
	pmxFile->header.model_comment_en = _pmx_readText(file);

//	printf("Model name: \n");
//	for (Sint32 m = 0; m < pmxFile->header.model_name.len; ++m) {
//		wprintf(L"%x ", (Uint8)pmxFile->header.model_name.data[m]);
//	}
//	printf("\n");
//	for (Sint32 m = 0; m < pmxFile->header.model_name_en.len; ++m) {
//		wprintf(L"%x ", (Uint8)pmxFile->header.model_name_en.data[m]);
//	}
//	printf("\n");

#ifdef PMX_DEBUG
	printf("~~~ PMX HEADER ~~~\n");
	printf("Text encoding:         %d", pmxFile->header.g_text_encoding);
	if (pmxFile->header.g_text_encoding) {
		printf(" (UTF-8)\n");
	} else {
		printf(" (UTF-16LE)\n");
	}
	printf("Additional vec4:       %d\n", pmxFile->header.g_additional_vec4_count);
	printf("Vertex index size:     %d\n", pmxFile->header.g_vertex_index_size);
	printf("Texture index size:    %d\n", pmxFile->header.g_texture_index_size);
	printf("Material index size:   %d\n", pmxFile->header.g_material_index_size);
	printf("Bone index size:       %d\n", pmxFile->header.g_bone_index_size);
	printf("Morph index size:      %d\n", pmxFile->header.g_morph_index_size);
	printf("Rigid body index size: %d\n", pmxFile->header.g_rigidbody_index_size);
#endif // PMX_DEBUG


	// Read vertices
	fread(&pmxFile->vertex_count, sizeof(Sint32), 1, file);
	pmxFile->vertices = (pmx_vertex*)malloc(sizeof(pmx_vertex) * pmxFile->vertex_count);
	for (Sint32 i = 0; i < pmxFile->vertex_count; ++i) {
		_pmx_readVertex(i, pmxFile, file);
	}

	// Read indices
	fread(&pmxFile->index_count, sizeof(Sint32), 1, file);
	pmxFile->indices = (Uint16*)malloc(sizeof(Uint16) * pmxFile->index_count);
	fread(pmxFile->indices, sizeof(Uint16), pmxFile->index_count, file);

	// Read textures
	fread(&pmxFile->texture_count, sizeof(Sint32), 1, file);
	pmxFile->textures = (pmx_texture*)malloc(sizeof(pmx_texture) * pmxFile->texture_count);
	for (Sint32 i = 0; i < pmxFile->texture_count; ++i) {
		_pmx_readTexture(i, pmxFile, file);
	}

	// Read materials
	fread(&pmxFile->material_count, sizeof(Sint32), 1, file);
	pmxFile->materials = (pmx_material*)malloc(sizeof(pmx_material) * pmxFile->material_count);
	for (Sint32 i = 0; i < pmxFile->material_count; ++i) {
		_pmx_readMaterial(i, pmxFile, file);
	}

	// Read bones
	fread(&pmxFile->bone_count, sizeof(Sint32), 1, file);
	pmxFile->bones = (pmx_bone*)malloc(sizeof(pmx_bone) * pmxFile->bone_count);
	for (Sint32 i = 0; i < pmxFile->bone_count; ++i) {
		_pmx_readBone(i, pmxFile, file);
	}


	fread(&pmxFile->morph_count, sizeof(Sint32), 1, file);
	pmxFile->morphs = (pmx_morph*)malloc(sizeof(pmx_morph) * pmxFile->morph_count);
	for (Sint32 i = 0; i < pmxFile->bone_count; ++i) {
		_pmx_readMorph(i, pmxFile, file);
	}


#ifdef PMX_DEBUG
	printf("Bones: %d\n", pmxFile->bone_count);
	printf("Morphs: %d\n", pmxFile->morph_count);
#endif // PMX_DEBUG

	fclose(file);
	return pmxFile;
}

void pmx_free(pmx_file* ptr) {
	free(ptr->header.model_name.data);
	free(ptr->header.model_name_en.data);
	free(ptr->header.model_comment.data);
	free(ptr->header.model_comment_en.data);
	for (Sint32 i = 0; i < ptr->vertex_count; ++i) {
		if(ptr->vertices[i].additional_vec4) {
			free(ptr->vertices[i].additional_vec4);
		}
	}
	free(ptr->vertices);

	free(ptr->indices);

	for (Sint32 i = 0; i < ptr->texture_count; ++i) {
		free(ptr->textures[i].path.data);
	}
	free(ptr->textures);

	for (Sint32 i = 0; i < ptr->material_count; ++i) {
		free(ptr->materials[i].name.data);
		free(ptr->materials[i].name_en.data);
		free(ptr->materials[i].comment.data);
	}
	free(ptr->materials);

	for (Sint32 i = 0; i < ptr->bone_count; ++i) {
		free(ptr->bones[i].name.data);
		free(ptr->bones[i].name_en.data);
//		if(ptr->bones[i].ik_link_count) {
//			free(ptr->bones[i].ik_links);
//		}
	}
	free(ptr->bones);

	free(ptr);
}
