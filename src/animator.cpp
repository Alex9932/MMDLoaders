/*
 * animator.cpp
 *
 *  Created on: Apr 17, 2022
 *      Author: alex9932
 */

#include "animator.h"
#include <engine/render/model.h>

Animator::Animator() { this->animation = NULL; }
Animator::~Animator() {}

void Animator::SetAnimation(Animation* anim) {
	this->animation = anim;
}

Animation* Animator::GetAnimation() {
	return animation;
}

void Animator::ApplayAnimation(Engine::Skeleton* skeleton) {
	ApplayAnimation(skeleton, skeleton->GetBone(0));
}

//void Animator::ApplayAnimation(Engine::Render::RiggedModel* model) {
void Animator::ApplayAnimation(Engine::Skeleton* skeleton, Engine::Bone* state) {
	if(animation == NULL) { return; }

	// Update skeleton
	for (Uint32 i = 0; i < skeleton->GetBoneCount(); ++i) {
		Engine::Bone* bone = &state[i];;
//		printf("Checking: %d %s\n", i, bone->name);
		BoneAnimationTrack* track = (BoneAnimationTrack*)animation->GetTrack(bone->name);
		if(track == NULL) { continue; }

		float off_x = bone->pos_offset.x;
		float off_y = bone->pos_offset.y;
		float off_z = bone->pos_offset.z;
		Engine::Math::vec3 pos = {off_x, off_y, off_z};
		Engine::Math::quat rot = {0, 0, 0, 1};
		if(track != NULL) {
			BoneState state = track->GetValue();
			pos.x += state.px;
			pos.y += state.py;
			pos.z += state.pz;
			rot.x = state.rx;
			rot.y = state.ry;
			rot.z = state.rz;
			rot.w = state.rw;
		}

		bone->position = pos;
		bone->rotation = rot;
	}
}

void Animator::ApplayFace(Engine::Render::RiggedModel* model, pmd_morph* morphs) {
	if(animation == NULL) { return; }
//
//	Engine::Render::Vertex* mdl_vertices = model->GetGeometryPtr();
//
//	// Update faces
//	pmd_morph* base = &morphs[0];
//	for (Uint32 i = 0; i < base->vertices; ++i) {
//		pmd_morph_vertex* base_vtx = &base->list[i];
//		mdl_vertices[base_vtx->vertex_id].vertex.x = -base_vtx->x;
//		mdl_vertices[base_vtx->vertex_id].vertex.y = base_vtx->y;
//		mdl_vertices[base_vtx->vertex_id].vertex.z = base_vtx->z;
//	}
//
//	for (Uint32 k = 1; k < MAX_FACES; ++k) {
//		FaceAnimationTrack* track = face_tracks[k];
//
//		Engine::Bone* bone = skeleton->GetBone(i);
//		FaceAnimationTrack* track = (FaceAnimationTrack*)animation->GetTrack(bone->name);
//
//		if(track == NULL) { continue; }
//
//		pmd_morph* morph = &morphs[k];
//		float c = track->GetValue();
//		if(c < 0) { c = 0; }
//		else if(c > 1) { c = 1; }
//
//		for (Uint32 i = 0; i < morph->vertices; ++i) {
//			pmd_morph_vertex* vtx = &morph->list[i];
//			pmd_morph_vertex* base_vtx = &base->list[vtx->vertex_id];
//			mdl_vertices[base_vtx->vertex_id].vertex.x += -vtx->x * c;
//			mdl_vertices[base_vtx->vertex_id].vertex.y += vtx->y * c;
//			mdl_vertices[base_vtx->vertex_id].vertex.z += vtx->z * c;
//		}
//	}
//
//	// Update vertex buffer
//	GLuint vbo = model->GetVBO();
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Engine::Render::Vertex) * model->VertexCount(), mdl_vertices);
}
