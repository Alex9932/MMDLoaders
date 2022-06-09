/*
 * renderer.cpp
 *
 *  Created on: May 10, 2022
 *      Author: alex9932
 */

#include "renderer.h"
#include "pm2_loader.h"

#include "rl3d.h"

#include "camera.h"

#include "mmd/pmd.h"

#include "gbuffer.h"
#include "postprocess.h"

#include <engine/render/window.h>
#include <engine/render/shader.h>
#include <engine/render/render.h>
#include <engine/render/render2d.h>
#include <engine/render/fontrenderer.h>
#include <engine/render/overlay.h>

#include <engine/core/input.h>
#include <engine/core/event.h>

#include <engine/level/level.h>

GLuint O_BUFFER;

static Engine::Render::Shader* shader;
static Engine::Camera* camera;
//static Engine::LookATCamera* camera;

static Engine::Render::Model* box_model;
static Engine::Render::Model* sphere_model;
static Engine::Render::Model* cylinder_model;

Uint32 box_model_id;
Uint32 sphere_model_id;
Uint32 cylinder_model_id;

static GLuint matrices_ubo;

// ~~~~~~~~~~~~~

static Engine::Math::mat4 MODEL_MATRIX;

static void _draw_coords(float scale, const Engine::Math::mat4& matrix, const Engine::Math::mat4& MODEL_MATRIX) {
	Engine::Math::vec4 c[] = {
			{1, 0, 0, 1},
			{1, 0, 0, 1},
			{0, 1, 0, 1},
			{0, 1, 0, 1},
			{0, 0, 1, 1},
			{0, 0, 1, 1},
	};
	Engine::Math::vec4 v[] = {
			{    0,     0,     0, 1},
			{scale,     0,     0, 1},
			{    0,     0,     0, 1},
			{    0, scale,     0, 1},
			{    0,     0,     0, 1},
			{    0,     0, scale, 1}
	};
	Engine::Math::vec4 ov[6];
	Engine::Math::mat4 a;
	for (Uint32 i = 0; i < 6; ++i) {
		Engine::Math::mat4_mul(&a, MODEL_MATRIX, matrix);
		Engine::Math::mat4_mul(&ov[i], v[i], a);
		cl_rl3d_vertex({ov[i].x, ov[i].y, ov[i].z, c[i].x, c[i].y, c[i].z, c[i].w});
	}
}

static void l3d(double delta, Engine::Skeleton* skeleton) {

//	Engine::Math::mat4 matrices[MAX_BONES];

	Engine::Math::mat4 IDENTITY;
	Engine::Math::mat4_identity(&IDENTITY);

//	for (Uint32 i = 0; i < pmd->bones_count; ++i) {
//		pmd_bone* bone = &pmd->bones[i];
//		if(bone->parent == -1) { // Root bone
//			matrices[i] = local_transforms[i];
//		} else {
//			Engine::Math::mat4_mul(&matrices[i], &matrices[bone->parent], &local_transforms[i]);
//		}
//	}

	cl_rl3d_applyMatrix(&IDENTITY);
	cl_rl3d_begin();
	_draw_coords(3, IDENTITY, IDENTITY);
	cl_rl3d_end();

	cl_rl3d_applyMatrix(&IDENTITY);

	cl_rl3d_begin();


	for (Uint32 i = 0; i < skeleton->GetBoneCount(); ++i) {
//		if(draw_ik && pmd_model->GetSkeleton()->GetBone(i).bone_type != 4) {
//			continue;
//		}

		Engine::Math::mat4 m = skeleton->GetBone(i)->transform;

		_draw_coords(0.4, m, MODEL_MATRIX);
	}
	cl_rl3d_end();
//	if(!draw_lines) {
//		return;
//	}

	cl_rl3d_begin();
	for (Uint32 i = 0; i < skeleton->GetBoneCount(); ++i) {
//		if(draw_ik && pmd->bones[i].bone_type != 4) {
//			continue;
//		}

		if(skeleton->GetBone(i)->parent != -1) {

			Engine::Bone* b1 = skeleton->GetBone(i);
			Engine::Bone* b2 = skeleton->GetBone(b1->parent);

			Engine::Math::mat4 m0;
			Engine::Math::mat4 m1;
			Engine::Math::mat4 _m0 = b1->transform;
			Engine::Math::mat4 _m1 = b2->transform;
//			if(anim) {
//				_m0 = matrices[pmd->bones[i].parent];
//				_m1 = matrices[i];
//			} else {
//				_m0 = bone_matrices[pmd->bones[i].parent];
//				_m1 = bone_matrices[i];
//			}

			Engine::Math::mat4_mul(&m0, MODEL_MATRIX, _m0);
			Engine::Math::mat4_mul(&m1, MODEL_MATRIX, _m1);

			float _x0 = m0.m03;
			float _y0 = m0.m13;
			float _z0 = m0.m23;
			float _x1 = m1.m03;
			float _y1 = m1.m13;
			float _z1 = m1.m23;

			cl_rl3d_vertex({_x0, _y0, _z0, 1, 1, 1, 1});
			cl_rl3d_vertex({_x1, _y1, _z1, 1, 1, 1, 1});
		}

	}
	cl_rl3d_end();

}

static void l3d_draw(double delta) {
	Engine::Level* level = Engine::GetLevel();
	for (Uint32 i = 0; i < level->GetObjectCount(); ++i) {
		Engine::Object* obj = level->GetObject(i);
		MODEL_MATRIX = obj->GetTransform();
		l3d(delta, Engine::Render::GetRegisteredRiggedModel(obj->GetModelID())->GetSkeleton());
	}
}


static void l3d_draw_text(Engine::Camera* camera, Engine::Skeleton* skeleton) {
	Engine::Math::mat4 IDENTITY;
//	Engine::Math::mat4 matrices[MAX_BONES];

	Engine::Math::mat4_identity(&IDENTITY);

//	for (Uint32 i = 0; i < pmd->bones_count; ++i) {
//		pmd_bone* bone = &pmd->bones[i];
//		if(bone->parent == -1) { // Root bone
//			matrices[i] = local_transforms[i];
//		} else {
//			Engine::Math::mat4_mul(&matrices[i], &matrices[bone->parent], &local_transforms[i]);
//		}
//	}

	char str[64];

	for (Uint32 i = 0; i < skeleton->GetBoneCount(); ++i) {
//		if(draw_ik && pmd->bones[i].bone_type != 4) {
//			continue;
//		}

		Engine::Math::mat4 m0 = skeleton->GetBone(i)->transform;
//		if(anim) {
//			m0 = matrices[i];
//		} else {
//			m0 = bone_matrices[i];
//		}

		// World space
		Engine::Math::vec4 pos = {m0.m03, m0.m13, m0.m23, 1};

		Engine::Math::mat4 view_projection;
		Engine::Math::mat4 mvp;
		Engine::Math::mat4_mul(&view_projection, &camera->GetProjection(), &camera->GetTransform());

		Engine::Math::mat4_mul(&mvp, view_projection, MODEL_MATRIX);

		// NDC space
		Engine::Math::vec4 proj_pos;
		Engine::Math::mat4_mul(&proj_pos, &pos, &mvp);

		Engine::Math::vec2 size;
		Engine::Window::GetWindowSize(&size);

		float x = proj_pos.x / proj_pos.w;
		float y = proj_pos.y / proj_pos.w;
		float z = proj_pos.z / proj_pos.w;

		if(z < 0) {
			continue;
		}

		if (x > 1 || x < -1 ||
			y > 1 || y < -1) {
			continue;
		}

		x = (x*0.5 + 0.5) * size.x;
		y = (y*0.5 + 0.5) * size.y;


		sprintf(str, "%d", i);
		// 131 22
		Engine::Render::RenderString(Engine::Render::GetDefaultFont(), str, {x, y}, {1, 1, 1, 1}, 0.25);

	}
}

// ~~~~~~~~~~~~~

static void UpdateGbuffer() {
	GBufferDestroy();
	GBufferInitialize();

	PPDestroy();
	PPInitialize();
}

static bool r_handler(SDL_Event* event) {

	if(event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
		Engine::Math::vec2 size;
		Engine::Window::GetWindowSize(&size);
		rgLogInfo(RG_LOG_RENDER, "Window resized %dx%d", (Uint32)size.x, (Uint32)size.y);
		UpdateGbuffer();
		camera->SetAspect(size.x / size.y);
		camera->UpdateProjection();
//		glViewport(0, 0, size.x, size.y);
	}

	return true;
}

void InitializeRenderer() {

	Engine::Math::vec2 size;
	Engine::Window::GetWindowSize(&size);

	shader = new Engine::Render::Shader("platform/shaders/base3d.vs", "platform/shaders/gbuffer_base.fs", NULL);
//	camera = new Engine::LookATCamera({0, 1.25, 0}, 5, Engine::Math::toradians(75), 0.1, 100, size.x / size.y);

	camera = new SmoothCamera({0, 1.25, 2}, Engine::Math::toradians(75), 0.1, 100, size.x / size.y);

	camera->SetRotation({0, 0, 0});
//	camera->DisableCollision();

	Engine::Event::RegisterEventHandler(r_handler);

	cl_rl3d_init();

	GBufferInitialize();
	PPInitialize();

	O_BUFFER = PPGetOutput();

	Engine::Allocator* alloc = new Engine::STDAllocator("PM2_ALLOCATOR");
	Engine::RegisterAllocator(alloc);
	PM2Loader* loader = new PM2Loader(alloc);

	Engine::Geometry* box_geom = loader->LoadGeometry("box.pm2");
	Engine::Geometry* sphere_geom = loader->LoadGeometry("sphere.pm2");
	Engine::Geometry* cylinder_geom = loader->LoadGeometry("cylinder.pm2");

	box_model = new Engine::Render::Model(box_geom);
	sphere_model = new Engine::Render::Model(sphere_geom);
	cylinder_model = new Engine::Render::Model(cylinder_geom);

	box_model_id = Engine::Render::RegisterModel(box_model);
	sphere_model_id = Engine::Render::RegisterModel(sphere_model);
	cylinder_model_id = Engine::Render::RegisterModel(cylinder_model);

//	TODO
//	loader->FreeGeometry(box_geom);
//	loader->FreeGeometry(sphere_geom);
//	loader->FreeGeometry(cylinder_geom);

//	free(box_geom->vertices);
//	free(sphere_geom->vertices);
//	free(cylinder_geom->vertices);
//	free(box_geom->indices);
//	free(sphere_geom->indices);
//	free(cylinder_geom->indices);
//	free(box_geom->materials);
//	free(sphere_geom->materials);
//	free(cylinder_geom->materials);
//	free(box_geom->indices_count);
//	free(sphere_geom->indices_count);
//	free(cylinder_geom->indices_count);
//	free(box_geom);
//	free(sphere_geom);
//	free(cylinder_geom);

	// Bones array
	glGenBuffers(1, &matrices_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Engine::Math::mat4) * PMD_MAX_BONES, NULL, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, matrices_ubo);

	delete loader;
}

void DestroyRenderer() {
	GBufferDestroy();
	PPDestroy();
	delete shader;
}

Engine::Camera* GetCamera() {
	return camera;
}

static Engine::Math::mat4 shader_matrices[PMD_MAX_BONES];

void RenderScene(Engine::Math::vec4& camera_angles, bool can_control, bool r_wireframe) {
//	if(can_control) {
//		float scroll_speed = 0.2;
//		if(Engine::Input::IsKeyDown(SDL_SCANCODE_LCTRL)) { scroll_speed = 0.5; }
//		if(Engine::Input::IsKeyDown(SDL_SCANCODE_LSHIFT)) { scroll_speed = 0.05; }
//
//		float mouse_wheel = Engine::Input::GetMouseDW() * scroll_speed;
//		camera->SetDistance(camera->GetDistance() - mouse_wheel);
//		if(Engine::Input::IsButtonDown(3)) {
//			camera->GetRotation().x += Engine::Input::GetMouseDY() * 0.1;
//			camera->GetRotation().y -= Engine::Input::GetMouseDX() * 0.1;
//		}
//
//		camera_angles.x = camera->GetRotation().x;
//		camera_angles.y = camera->GetRotation().y;
//		camera_angles.z = camera->GetRotation().z;
//		camera_angles.w = camera->GetDistance();
//
//	} else {
//		camera->GetRotation().x = camera_angles.x;
//		camera->GetRotation().y = camera_angles.y;
//		camera->GetRotation().z = camera_angles.z;
//		camera->SetDistance(camera_angles.w);
//	}

	camera->Update();

	Engine::Level* level = Engine::GetLevel();

	glBindFramebuffer(GL_FRAMEBUFFER, GBufferGetBuffer());
//	glClearColor(0.5, 0.5, 1, 1);
	glClearColor(0.1, 0.1, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, r_wireframe ? GL_LINE : GL_FILL);

	shader->Start();

	shader->SetMat4(shader->GetUniformLocation("proj"), &camera->GetProjection());
	shader->SetMat4(shader->GetUniformLocation("view"), &camera->GetTransform());

	shader->SetInt(shader->GetUniformLocation("t_unit0"), 0);
	shader->SetInt(shader->GetUniformLocation("t_unit1"), 1);

	shader->SetInt(shader->GetUniformLocation("anim"), 0);
	shader->SetUniformBlock(shader->GetUniformBlockIndex("BoneMatrices"), 0);

	Engine::Render::DisableBlend();

	Uint32 objects = level->GetObjectCount();
//	rgLogInfo(RG_LOG_RENDER, "Objects: %d", objects);
	for (Uint32 i = 0; i < objects; ++i) {
		Engine::Object* obj = level->GetObject(i);

		Engine::Render::RiggedModel* model = Engine::Render::GetRegisteredRiggedModel(obj->GetModelID());

		Engine::Skeleton* skeleton = model->GetSkeleton();
		Uint32 count = SDL_min(skeleton->GetBoneCount(), PMD_MAX_BONES);
		for (Uint32 i = 0; i < count; ++i) {
			Engine::Math::mat4_mul(&shader_matrices[i], skeleton->GetBone(i)->transform, skeleton->GetBone(i)->offset);
		}
		glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Engine::Math::mat4) * count, shader_matrices);

		shader->SetMat4(shader->GetUniformLocation("model"), &obj->GetTransform());
		shader->SetInt(shader->GetUniformLocation("anim"), 1);

		model->Draw(shader);

		for (Uint32 k = 0; k < model->GetMeshes().size(); ++k) {
			if(!model->GetMeshes()[k]->GetMaterial()->IsLoaded()) {
				model->GetMeshes()[k]->GetMaterial()->Load();
				break;
			}
		}
	}

	shader->Stop();

	Engine::Render::EnableBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	PPDoPostProcess(camera);

	// Reset framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Engine::Math::mat4 identity = Engine::Math::IDENTITY_MAT4;
	Engine::Math::vec2 size;

	Engine::Window::GetWindowSize(&size);

	Engine::Render::R2D_SetMatrix(identity);
	Engine::Render::R2D_BindTexture(O_BUFFER);

	Engine::Render::R2D_SetColor({1, 1, 1, 1});
	Engine::Render::R2D_DrawQuad({0, size.y}, {size.x, -size.y});


#if 0
	cl_rl3d_doRender(Engine::GetDeltaTime(), l3d_draw, camera);
//	for (Uint32 i = 0; i < level->GetObjectCount(); ++i) {
//		Engine::Object* obj = level->GetObject(i);
//		MODEL_MATRIX = obj->GetTransform();
//		l3d_draw_text(camera, Engine::Render::GetRegisteredRiggedModel(obj->GetModelID())->GetSkeleton());
//	}
#endif

}
