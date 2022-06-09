/*
 * rg_mmd_loader.cpp
 *
 *  Created on: Mar 31, 2022
 *      Author: alex9932
 */

#define _CRT_SECURE_NO_WARNINGS

#include "rg_mmd_loader.h"

#include <engine/core/utils.h>

// TODO: TESTING
#include "utils.h"

static void GetPath(char* dest, String src) {
	Uint32 sep = 0;
	Uint32 len = strlen(src);
	for (Uint32	i = len - 1; i >= 0; i--) {
		if(src[i] == '/') {
			sep = i;
			break;
		}
	}

	memcpy(dest, src, sep + 1);
}

Engine::RiggedGeometry* MMD_PMD_GeometryLoader::LoadGeometry(String path) {
	char p[128];
	GetPath(p, path);
	pmd_file* pmd = pmd_load(path);
	Engine::RiggedGeometry* geom = LoadGeometry(p, pmd);
	pmd_free(pmd);
	return geom;
}

Engine::RiggedGeometry* MMD_PMD_GeometryLoader::LoadGeometry(String path, pmd_file* pmd) {
	Engine::RiggedGeometry* geom = (Engine::RiggedGeometry*)allocator->Allocate(sizeof(Engine::RiggedGeometry));

	geom->materials = (Engine::Render::Material**)allocator->Allocate(sizeof(Engine::Render::Material*) * pmd->material_count);
	geom->indices_count = (Uint32*)allocator->Allocate(sizeof(Uint32) * pmd->material_count);

	char toon_texture[64];
	char spa_texture[64];
	char name[64];
	char name2[64];
	for (Uint32 i = 0; i < pmd->material_count; ++i) {
		Uint32 toon_index;
		if(pmd->materials[i].toon_number == 255) {
			toon_index = 0;
		} else {
			toon_index = pmd->materials[i].toon_number + 1;
		}
//		printf("Material: %s\n", pmd->materials[i].file_name);

		if(pmd->materials[i].file_name[0]) {
			memset(name, 0, 64);
			memset(name2, 0, 64);
			Sint32 j = 0;
			Sint32 k = 0;
			bool no_spa = false;
			for (; j < 20; ++j) {
				char c = pmd->materials[i].file_name[j];
				if(c == 0) { no_spa = true; }
				if(c == 0 || c == '*') { break; }
				if(c == '\\') { c = '/'; }
				name[j] = c;
			}
			for (; j < 20; ++j) {
				char c = pmd->materials[i].file_name[j];
				if(c == 0) { break; }
				if(c == '*') { continue; }
				if(c == '\\') { c = '/'; }
				name2[k] = c;
				k++;
			}

			memset(toon_texture, 0, 64);
			memset(spa_texture, 0, 64);
			sprintf(toon_texture, "&%s%s", path, name);
			sprintf(spa_texture, "&%s%s", path, name2);
			if(no_spa) {
				spa_texture[0] = 0;
			}
			//BuildTexturePath(toon_texture, &pmd->materials[i]);
		} else {
			sprintf(toon_texture, "&platform/toon/toon%02d.bmp", toon_index);
		}

		Engine::Render::Material* mat = new Engine::Render::Material(toon_texture, spa_texture);
		mat->SetColor({pmd->materials[i].colors.r, pmd->materials[i].colors.g, pmd->materials[i].colors.b, pmd->materials[i].colors.a});
		mat->SetAmbient({
			pmd->materials[i].colors.ambient_r * 2,
			pmd->materials[i].colors.ambient_g * 2,
			pmd->materials[i].colors.ambient_b * 2});
		mat->SetSpecular({pmd->materials[i].colors.specular_r, pmd->materials[i].colors.specular_g, pmd->materials[i].colors.specular_b});
		mat->SetShininess(pmd->materials[i].colors.shininess);
		geom->materials[i] = mat;
		geom->indices_count[i] = pmd->materials[i].surface_count;
	}

	geom->vertices = (Engine::Render::Vertex*)allocator->Allocate(sizeof(Engine::Render::Vertex) * pmd->vertex_count);
	geom->weights = (Engine::Render::Weight*)allocator->Allocate(sizeof(Engine::Render::Weight) * pmd->vertex_count);

	for (Uint32 i = 0; i < pmd->vertex_count; ++i) {
		// TODO
//		geom->vertices[i].vertex.x = pmd->vertices[i].vertex.x;
		geom->vertices[i].vertex.x = -pmd->vertices[i].vertex.x;
		geom->vertices[i].vertex.y = pmd->vertices[i].vertex.y;
		geom->vertices[i].vertex.z = pmd->vertices[i].vertex.z;
//		geom->vertices[i].normal.x = pmd->vertices[i].vertex.nx;
		geom->vertices[i].normal.x = -pmd->vertices[i].vertex.nx;
		geom->vertices[i].normal.y = pmd->vertices[i].vertex.ny;
		geom->vertices[i].normal.z = pmd->vertices[i].vertex.nz;
		geom->vertices[i].t_coord.x = pmd->vertices[i].vertex.u;
		geom->vertices[i].t_coord.y = pmd->vertices[i].vertex.v;

		// No tangents
		geom->vertices[i].tangent.x = 0;
		geom->vertices[i].tangent.y = 0;
		geom->vertices[i].tangent.z = 0;

		geom->weights[i].weights.x = (float)pmd->vertices[i].weight.b_weight[0] / 100.0f;
		geom->weights[i].weights.y = 1.0 - geom->weights[i].weights.x;
		geom->weights[i].weights.z = 0;
		geom->weights[i].weights.w = 0;
		geom->weights[i].id[0] = pmd->vertices[i].weight.b_id[0];
		geom->weights[i].id[1] = pmd->vertices[i].weight.b_id[1];
		geom->weights[i].id[2] = -1;
		geom->weights[i].id[3] = -1;

		if(geom->weights[i].weights.x + geom->weights[i].weights.y != 1) {
			printf("Weight error: %d => %f %f = %f\n", i, geom->weights[i].weights.x, geom->weights[i].weights.y, geom->weights[i].weights.x + geom->weights[i].weights.y);
		}
	}

	geom->indices = (Uint32*)allocator->Allocate(sizeof(Uint32) * pmd->index_count);
	for (Uint32 i = 0; i < pmd->index_count; ++i) {
		geom->indices[i] = pmd->indices[i];
	}

	geom->indices_total = pmd->index_count;
	geom->vertex_count = pmd->vertex_count;
	geom->meshes = pmd->material_count;

	return geom;
}

Engine::Skeleton* MMD_PMD_GeometryLoader::LoadSkeleton(pmd_file* pmd) {
	Engine::Skeleton* skeleton = new Engine::Skeleton(pmd->bones_count, pmd->ik_count);

	for (Uint32 i = 0; i < pmd->bones_count; ++i) {
		pmd_bone* bone = &pmd->bones[i];
		Engine::Math::vec3 b_offset;
		if(bone->parent == -1) { // Root bone
			// TODO
//			b_offset = {bone->x, bone->y, bone->z};
			b_offset = {-bone->x, bone->y, bone->z};
		} else {
			pmd_bone* parent = &pmd->bones[bone->parent];
			b_offset = {-(bone->x - parent->x), bone->y - parent->y, bone->z - parent->z};
//			b_offset = {bone->x - parent->x, bone->y - parent->y, bone->z - parent->z};
		}

		Engine::Math::mat4 offset;
		Engine::Math::mat4 bone_matrix;
		Engine::Math::mat4_identity(&bone_matrix);
		Engine::Math::mat4_translate(&bone_matrix, {-bone->x, bone->y, bone->z});
//		Engine::Math::mat4_translate(&bone_matrix, {bone->x, bone->y, bone->z});
		Engine::Math::mat4_inverse(&offset, &bone_matrix);
		Engine::Bone* sbone = skeleton->GetBone(i);
		memcpy(sbone->name, bone->name, 32);
//		printf("Reg: %d %s\n", i, sbone->name);
		sbone->id = i;
		sbone->parent = bone->parent;
		sbone->child = bone->child;
		sbone->pos_offset = b_offset;
		sbone->position = b_offset;
		sbone->rotation = {0, 0, 0, 1};
		sbone->transform = bone_matrix;
		sbone->offset = offset;
		sbone->has_limits = false;
		if(strstr(sbone->name, "ひざ") != NULL) {
			sbone->has_limits = true;
			sbone->limitation = {-1, 0, 0};
		}
	}

	for (Uint32 i = 0; i < pmd->ik_count; ++i) {
		Engine::BoneIK* ik = skeleton->GetBoneIK(i);
		pmd_ik_info* info = &pmd->ik[i];
		ik->angle_limit = info->angle_limit;
//		rgLogWarn(RG_LOG_GAME, "PMD: IK %d: %f", i, ik->angle_limit);
		ik->bones = info->bones;
		ik->effector = info->effector;
		ik->target = info->target;
		ik->max_iterations = info->max_iterations;
		for (Uint32 k = 0; k < ik->bones; ++k) {
			ik->list[k] = info->list[k];
		}
	}

//	for (Uint32 i = 0; i < pmd->bones_count; ++i) {
//		Engine::Math::vec3 pos = skeleton->GetBone(i)->pos_offset;
//		Engine::Math::quat rot = {0, 0, 0, 1};
//		skeleton->GetBone(i)->position = pos;
//		skeleton->GetBone(i)->rotation = rot;
//	}
//	skeleton->Rebuild();

	return skeleton;
}


Engine::RiggedGeometry* MMD_PMX_GeometryLoader::LoadGeometry(String path) {
	char p[128];
	GetPath(p, path);
	pmx_file* pmx = pmx_load(path);
	Engine::RiggedGeometry* geom = LoadGeometry(p, pmx);
	pmx_free(pmx);
	return geom;
}

Engine::RiggedGeometry* MMD_PMX_GeometryLoader::LoadGeometry(String path, pmx_file* pmx) {
	Engine::RiggedGeometry* geom = (Engine::RiggedGeometry*)malloc(sizeof(Engine::RiggedGeometry));

	char mat[64];
	char name[64];
	Uint32 k = 0;
	geom->materials = (Engine::Render::Material**)malloc(sizeof(Engine::Render::Material*) * pmx->material_count);
	geom->indices_count = (Uint32*)malloc(sizeof(Uint32) * pmx->material_count);
	for (Sint32 i = 0; i < pmx->material_count; ++i) {
		memset(name, 0, 64);
//		name[0] = '&';
		k = 0;
		pmx_text t = pmx->textures[pmx->materials[i].texture_id].path;
		for (Sint32 j = 0; j < t.len; ++j) {
			char c = t.data[j];
			if(c == '\\') { c = '/'; }

			if(c != 0) {
				name[k] = c;
				k++;
			}
		}
		if(k != 0) {
			sprintf(mat, "&%s%s", path, name);
		} else {
			sprintf(mat, "&%stoon%d.bmp", path, pmx->materials[i].toon_reference);
		}
//		printf("PMX Material %d %s: \n", k, mat);
		geom->materials[i] = new Engine::Render::Material(mat);
		geom->indices_count[i] = pmx->materials[i].surface_count;
	}

	geom->vertices = (Engine::Render::Vertex*)malloc(sizeof(Engine::Render::Vertex) * pmx->vertex_count);
	geom->weights = (Engine::Render::Weight*)malloc(sizeof(Engine::Render::Weight) * pmx->vertex_count);
	for (Sint32 i = 0; i < pmx->vertex_count; ++i) {
		geom->vertices[i].vertex.x = -pmx->vertices[i].position.x;
		geom->vertices[i].vertex.y = pmx->vertices[i].position.y;
		geom->vertices[i].vertex.z = pmx->vertices[i].position.z;
		geom->vertices[i].normal.x = -pmx->vertices[i].normal.x;
		geom->vertices[i].normal.y = pmx->vertices[i].normal.y;
		geom->vertices[i].normal.z = pmx->vertices[i].normal.z;
		geom->vertices[i].t_coord.x = pmx->vertices[i].uv.x;
		geom->vertices[i].t_coord.y = pmx->vertices[i].uv.y;

		// No tangents
		geom->vertices[i].tangent.x = 0;
		geom->vertices[i].tangent.y = 0;
		geom->vertices[i].tangent.z = 0;

		geom->weights[i].weights.x = pmx->vertices[i].weight.weights[0];
		geom->weights[i].weights.y = pmx->vertices[i].weight.weights[1];
		geom->weights[i].weights.z = pmx->vertices[i].weight.weights[2];
		geom->weights[i].weights.w = pmx->vertices[i].weight.weights[3];
		geom->weights[i].id[0] = pmx->vertices[i].weight.bone_id[0];
		geom->weights[i].id[1] = pmx->vertices[i].weight.bone_id[1];
		geom->weights[i].id[2] = pmx->vertices[i].weight.bone_id[2];
		geom->weights[i].id[3] = pmx->vertices[i].weight.bone_id[3];
	}

	geom->indices = (Uint32*)malloc(sizeof(Uint32) * pmx->index_count);
	for (Sint32 i = 0; i < pmx->index_count; ++i) {
		geom->indices[i] = pmx->indices[i];
	}

	geom->indices_total = pmx->index_count;
	geom->vertex_count = pmx->vertex_count;
	geom->meshes = pmx->material_count;

	return geom;
}

#define CHECK_FLAG(var, flag) ((var & flag) == flag)

Engine::Skeleton* MMD_PMX_GeometryLoader::LoadSkeleton(pmx_file* pmx) {

	// TODO: !!! OPTIMIZE THIS IF POSSIBLE !!!

	Uint32 ik_bones = 0;
	for (Sint32 i = 0; i < pmx->bone_count; ++i) {
		pmx_bone* bone = &pmx->bones[i];
		if(CHECK_FLAG(bone->flags, PMX_BONEFLAG_IK)) { ik_bones++; }
	}

	Engine::Skeleton* skeleton = new Engine::Skeleton(pmx->bone_count, ik_bones);

	for (Sint32 i = 0; i < pmx->bone_count; ++i) {
		pmx_bone* bone = &pmx->bones[i];

//		if(CHECK_FLAG(bone->flags, PMX_BONEFLAG_IK)) {
//			ik_bones++;
//		}

		Engine::Math::vec3 b_offset;
		if(bone->parent_id == -1) { // Root bone
			// TODO
//			b_offset = {bone->x, bone->y, bone->z};
			b_offset = {-bone->position.x, bone->position.y, bone->position.z};
		} else {
			pmx_bone* parent = &pmx->bones[bone->parent_id];
			b_offset = {-(bone->position.x - parent->position.x), bone->position.y - parent->position.y, bone->position.z - parent->position.z};
//			b_offset = {bone->x - parent->x, bone->y - parent->y, bone->z - parent->z};
		}

		Engine::Math::mat4 offset;
		Engine::Math::mat4 bone_matrix;
		Engine::Math::mat4_identity(&bone_matrix);
		Engine::Math::mat4_translate(&bone_matrix, {-bone->position.x, bone->position.y, bone->position.z});
//		Engine::Math::mat4_translate(&bone_matrix, {bone->x, bone->y, bone->z});
		Engine::Math::mat4_inverse(&offset, &bone_matrix);


		Engine::Bone* sbone = skeleton->GetBone(i);

//		Engine::Math::vec3 b_offset = {-bone->position.x, bone->position.y, bone->position.z};
//		Engine::Math::mat4 offset       = Engine::Math::IDENTITY_MAT4;
//		Engine::Math::mat4 bone_matrix  = Engine::Math::IDENTITY_MAT4;
//		Engine::Math::mat4 local_matrix = Engine::Math::IDENTITY_MAT4;
////		Engine::Math::mat4_identity(&bone_matrix);
//
//		if(bone->parent_id == -1) { // Root bone
//			Engine::Math::mat4_translate(&bone_matrix, b_offset);
//
////			sbone->pos_offset = b_offset;
//		} else {
//			// Apply parent transform
////			Engine::Math::mat4_identity(&local_matrix);
//			Engine::Math::mat4_translate(&local_matrix, b_offset);
//			Engine::Math::mat4 parent_matrix = skeleton->GetBone(bone->parent_id)->transform;
//			Engine::Math::mat4_mul(&bone_matrix, parent_matrix, local_matrix);
//
//		}
//
////		Engine::Math::mat4_translate(&bone_matrix, {bone->x, bone->y, bone->z});
//		Engine::Math::mat4_inverse(&offset, &bone_matrix);

		MMD_Utils::UTF16ToUTF8((WString)bone->name.data, bone->name.len);

//		printf("Bone %d: %s\n", i, MMD_Utils::GetBuffer());
		memcpy(sbone->name, MMD_Utils::GetBuffer(), 32);
//		printf("Reg: %d %s\n", i, sbone->name);
		sbone->id = i;
		sbone->parent = bone->parent_id;
		sbone->child = bone->inh_index;
		sbone->pos_offset = b_offset;
		sbone->position = b_offset;
		sbone->rotation = {0, 0, 0, 1};
		sbone->transform = bone_matrix;
//		sbone->transform = Engine::Math::IDENTITY_MAT4;
		sbone->offset = offset;
		sbone->has_limits = false;
		if(strstr(sbone->name, "ひざ") != NULL) {
			sbone->has_limits = true;
			sbone->limitation = {-1, 0, 0};
		}

//		if(i == 10) {
//			Engine::Helper::PrintMatrix(&bone_matrix);
//			Engine::Helper::PrintMatrix(&offset);
//		}
	}

//	skeleton->Rebuild();

//	Engine::Helper::PrintMatrix(&skeleton->GetBone(10)->transform);
//	Engine::Helper::PrintMatrix(&skeleton->GetBone(10)->offset);

//	rgLogWarn(RG_LOG_GAME, "PMX IK: %d", ik_bones);

	Uint32 ik_index = 0;
	for (Sint32 i = 0; i < pmx->bone_count; ++i) {
		pmx_bone* bone = &pmx->bones[i];
		if(CHECK_FLAG(bone->flags, PMX_BONEFLAG_IK)) {
			Engine::BoneIK* ik = skeleton->GetBoneIK(ik_index);

			ik->angle_limit = bone->ik_limit_radian;
//			rgLogWarn(RG_LOG_GAME, "IK %d: %f, Effector %d, target %d", ik_index, ik->angle_limit, i, bone->ik_target_index);
			ik->bones = bone->ik_link_count;
			ik->effector = bone->ik_target_index;
			ik->target = i;
			ik->max_iterations = bone->ik_loop_count;
			for (Uint32 k = 0; k < ik->bones; ++k) {
				ik->list[k] = bone->ik_links[k].bone_index;
			}

			ik_index++;
		}
//		Engine::BoneIK* ik = skeleton->GetBoneIK(i);
//		pmd_ik_info* info = &pmd->ik[i];
//		ik->angle_limit = info->angle_limit;
//		ik->bones = info->bones;
//		ik->effector = info->effector;
//		ik->target = info->target;
//		ik->max_iterations = info->max_iterations;
//		for (Uint32 k = 0; k < ik->bones; ++k) {
//			ik->list[k] = info->list[k];
//		}
	}

	return skeleton;
}


static Sint32 getFaceId(pmd_file* pmd, String name) {
	for (Uint32 i = 0; i < pmd->morph_count; ++i) {
		pmd_morph* morph = &pmd->morphs[i];
		bool ok = true;
		size_t len = strlen(morph->name);
		for (Uint32 k = 0; k < len; ++k) {
			if(name[k] == 0 && morph->name[k] == 0) { break; }
			if(name[k] != morph->name[k]) {
				ok = false;
				break;
			}
		}
		if(ok) { return i; }
	}
	return -1;
}

Animation* MMD_VMD_AnimationLoader::LoadCameraAnimation(vmd_file* vmd) {
	Animation* animation = new Animation();
	CameraAnimationTrack* camera_track = new CameraAnimationTrack(animation, "camera");
	animation->AddTrack(camera_track);

//	float scale = 0.1;

	Uint32 last_frame = 0;
	for (Sint32 i = 0; i < vmd->camera_count; ++i) {
		vmd_camera* camera = &vmd->cameras[i];
		if(last_frame < camera->frame) { last_frame = camera->frame; }

		Uint32 time = camera->frame;
		CameraState frame;
		// TODO
		frame.distance = -camera->distance;
		frame.tar_x = camera->target_x;
		frame.tar_y = camera->target_y;
		frame.tar_z = camera->target_z;
		frame.cr_x = -camera->camera_rx;
		frame.cr_y = camera->camera_ry;
		frame.cr_z = camera->camera_rz;
		frame.fov = camera->camera_fov;
		memcpy(frame.interpolation, camera->interpolation, 24);
		camera_track->AddKeyFrame(time, frame);
	}

	animation->SetLastFrame(last_frame);
	return animation;
}

#define MAX_BONES 1024
#define MAX_FACES 1024

Animation* MMD_VMD_AnimationLoader::LoadAnimation(pmd_file* pmd, vmd_file* vmd, Engine::Skeleton* skeleton) {
	Uint32 last_frame = 0;
	Animation* animation = new Animation();

	BoneAnimationTrack* bone_tracks[MAX_BONES];
	FaceAnimationTrack* face_tracks[MAX_FACES];
	for (Uint32 i = 0; i < MAX_BONES; ++i) { bone_tracks[i] = NULL; }
	for (Uint32 i = 0; i < MAX_FACES; ++i) { face_tracks[i] = NULL; }

	Uint32 k = 0;
	for (Sint32 i = 0; i < vmd->motion_count; ++i) {
		vmd_motion* motion = &vmd->motions[i];
		Engine::Bone* bone = skeleton->GetBoneByName(motion->bone_name);
		if(bone != NULL) {
			if(last_frame < motion->frame) { last_frame = motion->frame; }
			if(bone_tracks[bone->id] == NULL) {
//				printf("Name: {%d} %d %d %s %s\n", k, i, bone->id, motion->bone_name, bone->name);
				k++;
				bone_tracks[bone->id] = new BoneAnimationTrack(animation, bone->name, bone->id);
				animation->AddTrack(bone_tracks[bone->id]);
			}
			Uint32 time = motion->frame;
			BoneState frame;

			// TODO

			frame.px = -motion->position.x;
//			frame.px = motion->position.x;
			frame.py = motion->position.y;
			frame.pz = motion->position.z;
			frame.rx = motion->rotation.x;
			frame.ry = -motion->rotation.y;
			frame.rz = -motion->rotation.z;
//			frame.ry = motion->rotation.y;
//			frame.rz = motion->rotation.z;
			frame.rw = motion->rotation.w;
			memcpy(frame.interpolation, motion->interpolation, 64);
			bone_tracks[bone->id]->AddKeyFrame(time, frame);
		}
	}

//	printf("Loading morphs\n");
	for (Sint32 i = 0; i < vmd->face_count; ++i) {
		vmd_face* face = &vmd->faces[i];
		String face_name = face->name;
		Sint32 b_id = getFaceId(pmd, face_name);
		if(b_id != -1) {
//			printf("Morph: %d %s\n", b_id, face_name);
			if(face_tracks[b_id] == NULL) {
				face_tracks[b_id] = new FaceAnimationTrack(animation, face_name);
				animation->AddTrack(face_tracks[b_id]);
			}
			Uint32 time = face->frame;
			float frame = face->weight;
			face_tracks[b_id]->AddKeyFrame(time, frame);
		}
	}
//	printf("OK\n");

	animation->SetLastFrame(last_frame);

	return animation;
}
