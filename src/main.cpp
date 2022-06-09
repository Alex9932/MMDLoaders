/*
 * main.cpp
 *
 *  Created on: May 10, 2022
 *      Author: alex9932
 */

#define MODEL_PATH "mmd_models/"
#define PMD0_MODEL_PATH "mmd_models/Rin_Kagamine.pmd"
#define PMD1_MODEL_PATH "mmd_models/Len_Kagamine.pmd"

#define VPD0_FILE "resources/rin_1.vpd"
#define VPD1_FILE "resources/len_1.vpd"

#define PMX_MODEL_PATH "resources/apimiku/"
#define PMX_MODEL "resources/apimiku/Appearance Miku.pmx"

//#define PMX_MODEL_PATH "Izayoi_Sakuya_Ver2.10/"
//#define PMX_MODEL "Izayoi_Sakuya_Ver2.10/Izayoi_Sakuya_Ver2.10.pmx"

//#define PMX_MODEL_PATH "Gentoku Miku_Miku Expo 2021 ver1.10/"
//#define PMX_MODEL "Gentoku Miku_Miku Expo 2021 ver1.10/Gentoku Miku_Miku Expo 2021 ver1.10.pmx"

//#define PMX_MODEL_PATH "mdl1/"
//#define PMX_MODEL "mdl1/mdl1.pmx"

//#define PMX_MODEL_PATH "ouka/"
//#define PMX_MODEL "ouka/alice.pmx"
//#define PMX_MODEL "ouka/櫻花アリスv1.02_0PWhite.pmx"
//#define PMX_MODEL "ouka/櫻花アリスv1.02_1SRed.pmx"
//#define PMX_MODEL "ouka/櫻花アリスv1.02_2CBlue.pmx"
//#define PMX_MODEL "ouka/櫻花アリスv1.02_3AViolet.pmx"
//#define PMX_MODEL "ouka/櫻花アリスv1.02.pmx"



#define VPD2_FILE "resources/miku_3.vpd"
//#define VMD_FILE "miku_m3.vmd"

#include <engine/engine.h>
#include <engine/core/event.h>
#include <engine/core/filesystem.h>
#include <engine/core/math/math.h>
#include <engine/level/level.h>
#include <engine/render/window.h>
#include <engine/render/render.h>
#include <engine/render/render2d.h>

#include "mmd/pmx.h"
#include "mmd/pmd.h"
#include "mmd/rg_mmd_loader.h"

#include "mmd/vpd.h"


#include "renderer.h"
#include "postprocess.h"

#include <engine/core/ccdik.h>

#include <imgui.h>
//#include <imgui_impl_sdl.h>
//#include <imgui_impl_opengl3.h>

#include <imgui_neo_sequencer.h>
#include <ImGuizmo.h>

#if 1

static bool console = false;

static void ToggleConsole() {
	console = !console;
	if (console) { Engine::Render::ShowConsole(); }
	else { Engine::Render::HideConsole(); }
}

static bool Handler(SDL_Event* event) {

//	ImGui_ImplSDL2_ProcessEvent(event);

	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.scancode) {
		case SDL_SCANCODE_GRAVE:
			ToggleConsole();
			break;
		case SDL_SCANCODE_F11:
			Engine::Window::ToggleFullscreen();
			break;
		default:
			break;
		}
	}

	return true;
}


class Application : public Engine::BaseGame {

	public:
		Application() {
			this->is_client = true;
		}

		MMD_PMD_GeometryLoader* pmd_loader = NULL;
		MMD_PMX_GeometryLoader* pmx_loader = NULL;

		pmd_file* pmd0 = NULL;
		pmd_file* pmd1 = NULL;

		Engine::Render::RiggedModel* pmd_model0 = NULL;
		Engine::Render::RiggedModel* pmd_model1 = NULL;
		Engine::Render::RiggedModel* pmx_model = NULL;
		Engine::Entity* e0 = NULL;
		Engine::Entity* e1 = NULL;
		Engine::Entity* e2 = NULL;

		ImFont* font1 = NULL;

		Engine::Math::vec4 clear_color = {0, 0.5, 1, 1};
		Engine::Math::vec4 camera = {0, 0, 0, 3};

		bool can_control = true;
		bool r_wireframe = false;

		uint32_t currentFrame = 0;
		uint32_t startFrame = 0;
		uint32_t endFrame = 64;
		bool m_pTransformOpen = true;
		bool a = false;

		int b_id = 0;

		bool e0_selected = false;
		bool e1_selected = false;
		bool e2_selected = false;


		void ToRadians(Engine::Math::vec3& rot) {
			rot.x = -Engine::Math::toradians(rot.x);
			rot.y = -Engine::Math::toradians(rot.y);
			rot.z = -Engine::Math::toradians(rot.z);
		}

		void ManipulateEntity(Engine::Entity* e) {
//			Engine::Math::mat4 matrix = e->GetTransform();
//			Engine::Math::vec3 pos;
//			Engine::Math::vec3 rot;
//			Engine::Math::vec3 scale;
//
//			ImGuizmo::Manipulate(
//				(float*)&GetCamera()->GetTransform(),
//				(float*)&GetCamera()->GetProjection(),
////				ImGuizmo::TRANSLATE,
//				ImGuizmo::ROTATE | ImGuizmo::TRANSLATE,
//				ImGuizmo::WORLD,
//				(float*)&matrix,
////				(float*)&e0->GetTransform(),
//				NULL, NULL);
//
//			ImGuizmo::DecomposeMatrixToComponents(
//					(float*)&matrix,
//					(float*)&pos,
//					(float*)&rot,
//					(float*)&scale);
//
//			ToRadians(rot);
//			e->SetPosition(pos);
//			e->SetRotation(rot);
//			e->SetScale(scale);

			ImGuizmo::Manipulate(
				(float*)&GetCamera()->GetTransform(),
				(float*)&GetCamera()->GetProjection(),
				ImGuizmo::ROTATE | ImGuizmo::TRANSLATE,
				ImGuizmo::WORLD,
				(float*)&e->GetTransform(),
				NULL, NULL);
		}

//		static void decompose(Engine::Math::vec3* position, Engine::Math::quat* quaternion, Engine::Math::vec3* scale, const Engine::Math::mat4& matrix) {
//			Engine::Math::vec3 _x = {matrix.m00, matrix.m10, matrix.m20};
//			Engine::Math::vec3 _y = {matrix.m01, matrix.m11, matrix.m21};
//			Engine::Math::vec3 _z = {matrix.m02, matrix.m12, matrix.m22};
//			float sx = Engine::Math::vec3_length(&_x);
//			float sy = Engine::Math::vec3_length(&_y);
//			float sz = Engine::Math::vec3_length(&_z);
//
//			float det = Engine::Math::mat4_determinant(matrix);
//
//			if(det < 0) {sx = -sx; }
//
//			position->x = matrix.m03;
//			position->y = matrix.m13;
//			position->z = matrix.m23;
//
//			Engine::Math::mat4 _m;
//			SDL_memcpy(&_m, &matrix, sizeof(Engine::Math::mat4));
//
//			float invSX = 1.0 / sx;
//			float invSY = 1.0 / sy;
//			float invSZ = 1.0 / sz;
//
//			_m.m00 *= invSX;
//			_m.m10 *= invSX;
//			_m.m20 *= invSX;
//
//			_m.m01 *= invSY;
//			_m.m11 *= invSY;
//			_m.m21 *= invSY;
//
//			_m.m02 *= invSZ;
//			_m.m12 *= invSZ;
//			_m.m22 *= invSZ;
//
//			Engine::Math::quat_mat4(quaternion, _m);
//
//			scale->x = sx;
//			scale->y = sy;
//			scale->z = sz;
//		}

		Engine::Math::mat4 m;
		Engine::Math::mat4 m2;
		float scale = 0.1;
		Engine::Math::mat4 m1 = {
				scale, 0, 0, 0,
				0, scale, 0, 0,
				0, 0, scale, 0,
				0, 0, 0, scale
		};

		void ManipulateBone(Engine::Entity* e, Engine::Bone* bone) {

//			Engine::Math::mat4_mul(&m2, m1, bone->transform);
//			Engine::Math::mat4_mul(&m, m2, e->GetTransform());

//			Engine::Math::mat4_mul(&m2, bone->transform, e->GetTransform());
//			Engine::Math::mat4_mul(&m, m1, m2);

			m = Engine::Math::IDENTITY_MAT4;
			Engine::Math::mat4_mul(&m, bone->transform, m1);

			ImGuizmo::Manipulate(
				(float*)&GetCamera()->GetTransform(),
				(float*)&GetCamera()->GetProjection(),
				ImGuizmo::ROTATE | ImGuizmo::TRANSLATE,
				ImGuizmo::WORLD,
				(float*)&m,
				NULL, NULL);

			Engine::Math::vec3 pos;
			Engine::Math::quat rot;
			Engine::Math::vec3 scale;

			Engine::Math::mat4_decompose(&pos, &rot, &scale, m);

//			bone->position = pos;
			bone->rotation = rot;

		}

		void ApplyTransform(Engine::Entity* e) {
			Engine::Math::mat4 matrix = e->GetTransform();
			Engine::Math::vec3 pos;
			Engine::Math::vec3 rot;
			Engine::Math::vec3 scale;
			ImGuizmo::DecomposeMatrixToComponents(
					(float*)&matrix,
					(float*)&pos,
					(float*)&rot,
					(float*)&scale);

			ToRadians(rot);
			e->SetPosition(pos);
			e->SetRotation(rot);
			e->SetScale(scale);
		}

		void MainUpdate() {
//			Engine::Entity* ent = (Engine::Entity*)malloc(sizeof(Engine::Entity));
//
//			new(ent) Engine::Entity({0, 0, 0});
//
//			ent->SetScale(1);

			Engine::Render::EnableBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			Engine::Render::EnableDepthTest();

			Engine::Render::ClearColor(clear_color);
			Engine::Render::Clear();

//			pmd_model0->GetSkeleton()->Rebuild();
//			pmd_model1->GetSkeleton()->Rebuild();

			RenderScene(camera, can_control, r_wireframe);

			Engine::Render::DisableDepthTest();


//			Engine::Math::mat4 identity = Engine::Math::IDENTITY_MAT4;
//			Engine::Math::vec2 size;
//
//			Engine::Window::GetWindowSize(&size);
//
//			Engine::Render::R2D_SetMatrix(identity);
//			Engine::Render::R2D_BindTexture(0);
//
//			Engine::Render::R2D_SetColor({1, 1, 1, 1});
//			float m = 2;
//			Engine::Render::R2D_DrawQuad({100, 100}, {320 * m, 240 * m});



//			ImGui_ImplOpenGL3_NewFrame();
//			ImGui_ImplSDL2_NewFrame();
//			ImGui::NewFrame();
			Engine::Render::ImGuiBegin();

			ImGui::ShowMetricsWindow();
			ImGui::ShowStackToolWindow();
//			ImGui::ShowStyleEditor();
//			ImGui::ShowUserGuide();


			ImGui::PushFont(font1);

			ImGui::Begin("Controls");
			ImGui::Text("Camera settings");

			ImGui::Checkbox("Camera mouse control", &can_control);
			ImGui::SliderFloat("X axis", &camera.x, -RG_HALF_PI, RG_HALF_PI);
			ImGui::SliderFloat("Y axis", &camera.y, -RG_PI, RG_PI);
			ImGui::SliderFloat("Z axis", &camera.z, -RG_PI, RG_PI);
			ImGui::SliderFloat("Distance", &camera.w, 0.0f, 40.0f);

			ImGui::Text("Scene settings");
			ImGui::ColorEdit4("Clear color", (float*)&clear_color);
			ImGui::Checkbox("Wireframe", &r_wireframe);

			ImGui::Separator();
			ImGui::Text("Framebuffer");
			ImGui::InputInt("Output", (int*)&O_BUFFER, 1, 10);
			if(ImGui::Button("Reset output")) {
				O_BUFFER = PPGetOutput();
			}

			ImGui::Separator();
			ImGui::Checkbox("Entity 0", &e0_selected);
			if(ImGui::Button("Apply transform e0")) {
				ApplyTransform(e0);
//				rgLogWarn(RG_LOG_SYSTEM, "Apply!");
			}

			ImGui::Separator();
			ImGui::Checkbox("Entity 1", &e1_selected);
			if(ImGui::Button("Apply transform e1")) {
				ApplyTransform(e1);
//				rgLogWarn(RG_LOG_SYSTEM, "Apply!2");
			}

			ImGui::Separator();
			ImGui::Checkbox("Entity 2", &e2_selected);
			if(ImGui::Button("Apply transform e2")) {
				ApplyTransform(e2);
//				rgLogWarn(RG_LOG_SYSTEM, "Apply!2");
			}

			ImGui::Separator();
			ImGui::ListBox("Bone", &b_id, pmd0->e_bones, pmd0->bones_count);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::End();



			ImGui::Begin("Timeline", &a, ImGuiWindowFlags_NoMove);

			if(ImGui::BeginNeoSequencer("Sequencer", &currentFrame, &startFrame, &endFrame, {0, 0},
					ImGuiNeoSequencerFlags_AllowSelection |
					ImGuiNeoSequencerFlags_AllowLengthChanging
					)) {
//			ImGui::BeginNeoSequencer("Sequencer", &currentFrame, &startFrame, &endFrame);
				if(ImGui::BeginNeoGroup("Transform", &m_pTransformOpen)) {
				    std::vector<uint32_t> keys = {0, 10, 24};
				    if(ImGui::BeginNeoTimeline("Position", keys )) {
				        ImGui::EndNeoTimeLine();
				    }
				    if(ImGui::BeginNeoTimeline("Rotation", keys )) {
				        ImGui::EndNeoTimeLine();
				    }
				    if(ImGui::BeginNeoTimeline("scale", keys )) {
				        ImGui::EndNeoTimeLine();
				    }
				    ImGui::EndNeoTimeLine();
				}
			    ImGui::EndNeoSequencer();
			}


			ImGui::PopFont();
			ImGui::End();

			ImGuizmo::BeginFrame();

			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);


//			ManipulateBone(e0, pmd_model0->GetSkeleton()->GetBone(b_id));


			if(e0_selected) { ManipulateEntity(e0); } else { e0->Update(); }
			if(e1_selected) { ManipulateEntity(e1); } else { e1->Update(); }
			if(e2_selected) { ManipulateEntity(e2); } else { e2->Update(); }


			Engine::Render::ImGuiEnd();


//			ImGui::Render();
//			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


			Engine::Render::Render();
		}

		void Initialize() {
			// Initializу

			InitializeRenderer();

			ImGuiIO& io = ImGui::GetIO();
			font1 = io.Fonts->AddFontFromFileTTF("platform/msyh.ttf", 18);

			Engine::Filesystem::Mount("resources/mmd_pack.rfs");

			Engine::Allocator* alloc = new Engine::STDAllocator("MMD_GEOMETRY_ALLOC");
			Engine::RegisterAllocator(alloc);

			pmd_loader = new MMD_PMD_GeometryLoader(alloc);
			pmx_loader = new MMD_PMX_GeometryLoader(alloc);

			pmd0 = pmd_load(PMD0_MODEL_PATH);
			pmd1 = pmd_load(PMD1_MODEL_PATH);

			Engine::RiggedGeometry* geom0 = pmd_loader->LoadGeometry(MODEL_PATH, pmd0);
			Engine::RiggedGeometry* geom1 = pmd_loader->LoadGeometry(MODEL_PATH, pmd1);
			Engine::Skeleton* skeleton0 = pmd_loader->LoadSkeleton(pmd0);
			Engine::Skeleton* skeleton1 = pmd_loader->LoadSkeleton(pmd1);
			pmd_model0 = new Engine::Render::RiggedModel(geom0, skeleton0);
			pmd_model1 = new Engine::Render::RiggedModel(geom1, skeleton1);
			pmd_loader->FreeGeometry(geom0);
			pmd_loader->FreeGeometry(geom1);


			pmx_file* pmx = pmx_load(PMX_MODEL);
			Engine::RiggedGeometry* geom2 = pmx_loader->LoadGeometry(PMX_MODEL_PATH, pmx);
			Engine::Skeleton* skeleton2 = pmx_loader->LoadSkeleton(pmx);
			pmx_model = new Engine::Render::RiggedModel(geom2, skeleton2);
//			pmx_loader->FreeGeometry(geom2);
//			free(geom0->materials);
//			free(geom0->vertices);
//			free(geom0->weights);
//			free(geom0->indices);
//			free(geom0->indices_count);
//			free(geom0);
//			free(geom1->materials);
//			free(geom1->vertices);
//			free(geom1->weights);
//			free(geom1->indices);
//			free(geom1->indices_count);
//			free(geom1);

			Uint32 id0 = Engine::Render::RegisterRiggedModel(pmd_model0);
			Uint32 id1 = Engine::Render::RegisterRiggedModel(pmd_model1);
			Uint32 id2 = Engine::Render::RegisterRiggedModel(pmx_model);


			// IMGUI
//			ImGui::CreateContext();
//			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::StyleColorsClassic();
//
//			ImGui_ImplSDL2_InitForOpenGL(Engine::Window::GetWindow(), NULL);
//			ImGui_ImplOpenGL3_Init("#version 150");


//			Engine::Window::ToggleFullscreen();
			Engine::Event::RegisterEventHandler(Handler);
			Engine::Render::ShowProfiler();


			Engine::NewLevel();

			Engine::Level* level = Engine::GetLevel();

			e0 = new Engine::Entity({-1.1, 0, 0.0});
			e0->SetModelID(id0);
			e0->SetScale(0.1);
			e0->SetRotation({0, RG_PI, 0});
			level->SpawnEntity(e0);

			e1 = new Engine::Entity({ 1.1, 0, 0.0});
			e1->SetModelID(id1);
			e1->SetScale(0.1);
			e1->SetRotation({0, RG_PI, 0});
			level->SpawnEntity(e1);


			e2 = new Engine::Entity({ 0.0, 0, 0.0});
			e2->SetModelID(id2);
			e2->SetScale(0.1);
			e2->SetRotation({0, RG_PI, 0});
			level->SpawnEntity(e2);



			Engine::Skeleton* skel = pmd_model0->GetSkeleton();
			Engine::Skeleton* skel2 = pmd_model1->GetSkeleton();
			vpd_pose* vpd = vpd_fromFile(VPD0_FILE);
			vpd_pose* vpd2 = vpd_fromFile(VPD1_FILE);

			for (Uint32 i = 0; i < vpd->bone_count; ++i) {
				vpd_bone* bone = &vpd->bones[i];
				Engine::Bone* sb = skel->GetBone(bone->id);
				sb->position.x = sb->pos_offset.x - bone->position.x;
				sb->position.y = sb->pos_offset.y + bone->position.y;
				sb->position.z = sb->pos_offset.z + bone->position.z;
				sb->rotation.x = bone->rotation.x;
				sb->rotation.y = -bone->rotation.y;
				sb->rotation.z = -bone->rotation.z;
				sb->rotation.w = bone->rotation.w;
			}

			for (Uint32 i = 0; i < vpd2->bone_count; ++i) {
				vpd_bone* bone = &vpd2->bones[i];
				Engine::Bone* sb = skel2->GetBone(bone->id);
				sb->position.x = sb->pos_offset.x - bone->position.x;
				sb->position.y = sb->pos_offset.y + bone->position.y;
				sb->position.z = sb->pos_offset.z + bone->position.z;
				sb->rotation.x = bone->rotation.x;
				sb->rotation.y = -bone->rotation.y;
				sb->rotation.z = -bone->rotation.z;
				sb->rotation.w = bone->rotation.w;
			}

//			for (Uint32 i = 0; i < vmd->bone_count; ++i) {
//				vpd_bone* bone = &vmd->bones[i];
//				Engine::Bone* sb = skel3->GetBone(bone->id);
//				sb->position.x = sb->pos_offset.x - bone->position.x;
//				sb->position.y = sb->pos_offset.y + bone->position.y;
//				sb->position.z = sb->pos_offset.z + bone->position.z;
//				sb->rotation.x = bone->rotation.x;
//				sb->rotation.y = -bone->rotation.y;
//				sb->rotation.z = -bone->rotation.z;
//				sb->rotation.w = bone->rotation.w;
//			}


			Engine::CCDIK_Solver* solver = new Engine::CCDIK_Solver();

			// Apply VPD pose
			skel->Rebuild();
			// Solve IK
			solver->Solve(skel);
			// Rebuild solved skeleton
			skel->Rebuild();

			// Apply VPD pose
			skel2->Rebuild();
			// Solve IK
			solver->Solve(skel2);
			// Rebuild solved skeleton
			skel2->Rebuild();






			Engine::Skeleton* skel3 = pmx_model->GetSkeleton();
//			vmd_file* vmd = vmd_load(VMD_FILE);
//			rgLogInfo(RG_LOG_GAME, "VMD: %d", vmd->motion_count);
//			for (Sint32 i = 0; i < vmd->motion_count; ++i) {
//				vmd_motion* motion = &vmd->motions[i];
//
//				Engine::Bone* b = skel3->GetBoneByName(motion->bone_name);
////				rgLogInfo(RG_LOG_GAME, "VMD: Apply %s => %s", motion->bone_name, b->name);
//
//				b->position.x = b->pos_offset.x - motion->position.x;
//				b->position.y = b->pos_offset.y + motion->position.y;
//				b->position.z = b->pos_offset.z + motion->position.z;
//				b->rotation.x = motion->rotation.x;
//				b->rotation.y = -motion->rotation.y;
//				b->rotation.z = -motion->rotation.z;
//				b->rotation.w = motion->rotation.w;
//			}


			vpd_pose* vpd3 = vpd_fromFile(VPD2_FILE);
			for (Uint32 i = 0; i < vpd3->bone_count; ++i) {
				vpd_bone* bone = &vpd3->bones[i];
//				Engine::Bone* sb = skel3->GetBone(bone->id);
				Engine::Bone* sb = skel3->GetBoneByName(bone->bone_name);
				if(sb == NULL) { continue; }
//				rgLogInfo(RG_LOG_GAME, "VMD: Apply %s => (%d)%s", bone->bone_name, sb->id, sb->name);
				sb->position.x = sb->pos_offset.x - bone->position.x;
				sb->position.y = sb->pos_offset.y + bone->position.y;
				sb->position.z = sb->pos_offset.z + bone->position.z;
				sb->rotation.x = bone->rotation.x;
				sb->rotation.y = -bone->rotation.y;
				sb->rotation.z = -bone->rotation.z;
				sb->rotation.w = bone->rotation.w;
			}


//			Engine::Bone* b = skel3->GetBone(9);
//			b->rotation = {0, 0, 0.72, 0.689};
//			b->position = {0, 0, 0};//b->pos_offset;

			// Apply VPD pose
			skel3->Rebuild();
			// Solve IK
			solver->Solve(skel3);
			// Rebuild solved skeleton
			skel3->Rebuild();

			delete solver;

		}

		void Quit() {
			// Clean up

			DestroyRenderer();
//
//			ImGui_ImplOpenGL3_Shutdown();
//			ImGui_ImplSDL2_Shutdown();
////			ImGui::DestroyContext();
		}

};

int main(int argc, char** argv) {

	if(Engine::ProcessArguments(argc, (String*)argv)) {
		printf("Arguments error!\n");
		return -1;
	}

	Application game;
	Engine::Initialize(game);
	Engine::Start();
	return 0;
}

#endif
