/*
 * animation.cpp
 *
 *  Created on: Apr 7, 2022
 *      Author: alex9932
 */

#include "animation.h"

#include <engine/engine.h>
#include <engine/core/strings.h>
#include <engine/core/math/math.h>
#include <engine/core/math/matrix.h>
#include <engine/core/math/vector.h>

#include <iostream>
#include <algorithm>
//#include <bits/stdc++.h>

struct CameraInterpolationValues {
	float x; // Camera's X coordinate
	float y; // Camera's Y coordinate
	float z; // Camera's Z coordinate
	float r; // Camera's rotation
	float d; // Camera's distance
	float f; // Camera's field of view
};

RG_FORCE_INLINE static float lerp(float a, float b, float t) {
	return a * (1 - t) + b * t;
}

RG_FORCE_INLINE static float cerp(float a, float b, float t) {
	double m = (1-SDL_cos(t*RG_PI))/2;
	return lerp(a, b, m);
}

static double bezier(const Engine::Math::vec2& X1, const Engine::Math::vec2& X2, double x) {
	float t = 0.5f;
	float s = 0.5f;
	for (int i = 0; i < 15; i++) {
		float zero = (3 * s * s * t * X1.x) + (3 * s * t * t * X2.x) + (t * t * t) - x;
		if (SDL_fabsf(zero) < 0.00001f) { break; }
		if (zero > 0) { t -= 1 / (4 * SDL_powf(2, i)); }
		else { t += 1 / (4 * SDL_powf(2, i)); }
		s = 1 - t;
	}
	return (3 * s * s * t * X1.y) + (3 * s * t * t * X2.y) + (t * t * t);
}

static void GetBoneInterpolationValue(Engine::Math::vec4* vec, BoneState* state, double dt) {
	Engine::Math::vec2 X1, X2;
	Engine::Math::vec2 Y1, Y2;
	Engine::Math::vec2 Z1, Z2;
	Engine::Math::vec2 R1, R2;
	X1.x = state->interpolation[0] / 127.0f;
	X1.y = state->interpolation[4] / 127.0f;
	X2.x = state->interpolation[8] / 127.0f;
	X2.y = state->interpolation[12] / 127.0f;
	Y1.x = state->interpolation[16] / 127.0f;
	Y1.y = state->interpolation[20] / 127.0f;
	Y2.x = state->interpolation[24] / 127.0f;
	Y2.y = state->interpolation[28] / 127.0f;
	Z1.x = state->interpolation[32] / 127.0f;
	Z1.y = state->interpolation[36] / 127.0f;
	Z2.x = state->interpolation[40] / 127.0f;
	Z2.y = state->interpolation[44] / 127.0f;
	R1.x = state->interpolation[48] / 127.0f;
	R1.y = state->interpolation[52] / 127.0f;
	R2.x = state->interpolation[56] / 127.0f;
	R2.y = state->interpolation[60] / 127.0f;
	vec->x = bezier(X1, X2, dt);
	vec->y = bezier(Y1, Y2, dt);
	vec->z = bezier(Z1, Z2, dt);
	vec->w = bezier(R1, R2, dt);
}

static void GetCameraInterpolationValue(CameraInterpolationValues* vec, CameraState* state, double dt) {
	Engine::Math::vec2 X1, X2;
	Engine::Math::vec2 Y1, Y2;
	Engine::Math::vec2 Z1, Z2;
	Engine::Math::vec2 R1, R2;
	Engine::Math::vec2 D1, D2;
	Engine::Math::vec2 F1, F2;
	X1.x = state->interpolation[0] / 127.0f;
	X1.y = state->interpolation[1] / 127.0f;
	X2.x = state->interpolation[2] / 127.0f;
	X2.y = state->interpolation[3] / 127.0f;
	Y1.x = state->interpolation[4] / 127.0f;
	Y1.y = state->interpolation[5] / 127.0f;
	Y2.x = state->interpolation[6] / 127.0f;
	Y2.y = state->interpolation[7] / 127.0f;
	Z1.x = state->interpolation[8] / 127.0f;
	Z1.y = state->interpolation[9] / 127.0f;
	Z2.x = state->interpolation[10] / 127.0f;
	Z2.y = state->interpolation[11] / 127.0f;
	R1.x = state->interpolation[12] / 127.0f;
	R1.y = state->interpolation[13] / 127.0f;
	R2.x = state->interpolation[14] / 127.0f;
	R2.y = state->interpolation[15] / 127.0f;
	D1.x = state->interpolation[16] / 127.0f;
	D1.y = state->interpolation[17] / 127.0f;
	D2.x = state->interpolation[18] / 127.0f;
	D2.y = state->interpolation[19] / 127.0f;
	F1.x = state->interpolation[20] / 127.0f;
	F1.y = state->interpolation[21] / 127.0f;
	F2.x = state->interpolation[22] / 127.0f;
	F2.y = state->interpolation[23] / 127.0f;
	vec->x = bezier(X1, X2, dt);
	vec->y = bezier(Y1, Y2, dt);
	vec->z = bezier(Z1, Z2, dt);
	vec->r = bezier(R1, R2, dt);
	vec->d = bezier(D1, D2, dt);
	vec->f = bezier(F1, F2, dt);
}


// Classes implementation

BoneAnimationTrack::BoneAnimationTrack(class Animation* anim, String name, Uint32 id): AnimationTrack(anim, name) { this->id = id; }
BoneAnimationTrack::~BoneAnimationTrack() {}

#define BONE_ITERATOR std::map<Uint32, BoneState>::iterator

void BoneAnimationTrack::Update() {

	BONE_ITERATOR prew = key_frames.begin();
	BONE_ITERATOR next = key_frames.begin();
	for(BONE_ITERATOR i = key_frames.begin(); i != key_frames.end(); i++) {
		if(i->first > animation->GetAnimationTime()) {
			next = i;
			break;
		}
		prew = i;
	}
	Uint32 delta = next->first - prew->first;
	double anim_dt = (animation->GetAnimationTime() - prew->first) / delta;

//	Engine::Math::vec4 dt = GetInterpolationValue(&next->second, anim_dt);
	Engine::Math::vec4 dt;
//	GetBoneInterpolationValue(&dt, &prew->second, anim_dt);
	GetBoneInterpolationValue(&dt, &next->second, anim_dt);

	if(delta == 0) {
		dt.x = 0;
		dt.y = 0;
		dt.z = 0;
		dt.w = 0;
		anim_dt = 0;
	}

	BoneState* b_prew = &prew->second;
	BoneState* b_next = &next->second;

	Engine::Math::vec3 p_prew = {b_prew->px, b_prew->py, b_prew->pz};
	Engine::Math::quat q_prew = {b_prew->rx, b_prew->ry, b_prew->rz, b_prew->rw};
	Engine::Math::vec3 p_next = {b_next->px, b_next->py, b_next->pz};
	Engine::Math::quat q_next = {b_next->rx, b_next->ry, b_next->rz, b_next->rw};

	Engine::Math::vec3 p_current;
	Engine::Math::quat q_current;

#if 1
	p_current.x = lerp(p_prew.x, p_next.x, dt.x);
	p_current.y = lerp(p_prew.y, p_next.y, dt.y);
	p_current.z = lerp(p_prew.z, p_next.z, dt.z);
	Engine::Math::quat_slerp(&q_current, q_prew, q_next, dt.w);
#else
	p_current.x = cerp(p_prew.x, p_next.x, dt.x);
	p_current.y = cerp(p_prew.y, p_next.y, dt.y);
	p_current.z = cerp(p_prew.z, p_next.z, dt.z);
	float ki = cerp(0, 1, dt.w);
	Engine::Math::quat_slerp(&q_current, q_prew, q_next, ki);
#endif

//	if(id == 5) {
//		rgLogInfo(RG_LOG_GAME, "%f %f %f [%f %f %f => %f %f %f] %d %d %d (%lf %lf %lf)\n",
//				p_current.x, p_current.y, p_current.z,
//				p_prew.x, p_prew.y, p_prew.z,
//				p_next.x, p_next.y, p_next.z,
//				(Uint32)animation->GetAnimationTime(), prew->first, next->first,
//				dt.x, dt.y, dt.z);
//	}

	state.px = p_current.x;
	state.py = p_current.y;
	state.pz = p_current.z;
	state.rx = q_current.x;
	state.ry = q_current.y;
	state.rz = q_current.z;
	state.rw = q_current.w;
}

static bool b_cmp(std::pair<Uint32, BoneState>& a, std::pair<Uint32, BoneState>& b) {
	 return a.first < b.first;
}

static void b_sort(std::map<Uint32, BoneState>& M) {
	std::vector<std::pair<Uint32, BoneState>> A;
	for (auto& it : M) {
		A.push_back(it);
	}
	std::sort(A.begin(), A.end(), b_cmp);
	M.clear();
	for (auto& it : A) {
		M[it.first] = it.second;
	}
}

void BoneAnimationTrack::AddKeyFrame(Uint32 time, BoneState frame) {
	key_frames[time] = frame;

	// TODO: remove this code
	b_sort(key_frames);
}

BoneState BoneAnimationTrack::GetValue() {
	return state;
}

std::map<Uint32, BoneState>& BoneAnimationTrack::GetKeyFrames() {
	return key_frames;
}


FaceAnimationTrack::FaceAnimationTrack(class Animation* anim, String name): AnimationTrack(anim, name) {}
FaceAnimationTrack::~FaceAnimationTrack() {}

#define FACE_ITERATOR std::map<Uint32, float>::iterator

void FaceAnimationTrack::Update() {

	FACE_ITERATOR prew = key_frames.begin();
	FACE_ITERATOR next = key_frames.begin();
	for(FACE_ITERATOR i = key_frames.begin(); i != key_frames.end(); i++) {
		if(i->first > animation->GetAnimationTime()) {
			next = i;
			break;
		}
		prew = i;
	}
	Uint32 delta = next->first - prew->first;
	if(delta == 0) {
		weight = 0;
	} else {
		double dt = (animation->GetAnimationTime() - prew->first) / delta;
		weight = cerp(prew->second, next->second, dt);
	}

}

static bool f_cmp(std::pair<Uint32, float>& a, std::pair<Uint32, float>& b) {
	 return a.first < b.first;
}

static void f_sort(std::map<Uint32, float>& M) {
	std::vector<std::pair<Uint32, float>> A;
	for (auto& it : M) {
		A.push_back(it);
	}
	std::sort(A.begin(), A.end(), f_cmp);
	M.clear();
	for (auto& it : A) {
		M[it.first] = it.second;
	}
}

void FaceAnimationTrack::AddKeyFrame(Uint32 time, float weight) {
	key_frames[time] = weight;

	// TODO: remove this code
	f_sort(key_frames);
}

float FaceAnimationTrack::GetValue() {
	return weight;
}


CameraAnimationTrack::CameraAnimationTrack(class Animation* anim, String name): AnimationTrack(anim, name) {}
CameraAnimationTrack::~CameraAnimationTrack() {}

#define CAMERA_ITERATOR std::map<Uint32, CameraState>::iterator

void CameraAnimationTrack::Update() {

	CAMERA_ITERATOR prew = key_frames.begin();
	CAMERA_ITERATOR next = key_frames.begin();
	for(CAMERA_ITERATOR i = key_frames.begin(); i != key_frames.end(); i++) {
		if(i->first > animation->GetAnimationTime()) {
			next = i;
			break;
		}
		prew = i;
	}
	Uint32 delta = next->first - prew->first;

//	CAMERA_ITERATOR prew = key_frames.begin();
//	CAMERA_ITERATOR next = key_frames.begin();
//	for(CAMERA_ITERATOR i = key_frames.begin(); i != key_frames.end(); i++) {
//		if(i->first > animation->GetAnimationTime()) {
//			next = i;
//			break;
//		}
//		prew = i;
//	}
//	Uint32 delta = next->first - prew->first;
	if(delta == 0) {
		state.distance = 0;
		state.tar_x = 0;
		state.tar_y = 0;
		state.tar_z = 0;
		state.cr_x = 0;
		state.cr_y = 0;
		state.cr_z = 0;
		state.fov = 0;
	} else {
		double dt = (animation->GetAnimationTime() - prew->first) / delta;
		CameraInterpolationValues cam_dt;
//		GetCameraInterpolationValue(&cam_dt, &next->second, dt);
		GetCameraInterpolationValue(&cam_dt, &prew->second, dt);

#define INTERPOLATION lerp
//#define INTERPOLATION cerp

//		state.distance = INTERPOLATION(prew->second.distance, next->second.distance, cam_dt.d);
//		state.tar_x = INTERPOLATION(prew->second.tar_x, next->second.tar_x, cam_dt.x);
//		state.tar_y = INTERPOLATION(prew->second.tar_y, next->second.tar_y, cam_dt.y);
//		state.tar_z = INTERPOLATION(prew->second.tar_z, next->second.tar_z, cam_dt.z);
//		state.cr_x = INTERPOLATION(prew->second.cr_x, next->second.cr_x, cam_dt.r);
//		state.cr_y = INTERPOLATION(prew->second.cr_y, next->second.cr_y, cam_dt.r);
//		state.cr_z = INTERPOLATION(prew->second.cr_z, next->second.cr_z, cam_dt.r);
//		state.fov = INTERPOLATION(prew->second.fov, next->second.fov, cam_dt.f);

		state.distance = INTERPOLATION(prew->second.distance, next->second.distance, dt);
		state.tar_x = INTERPOLATION(prew->second.tar_x, next->second.tar_x, dt);
		state.tar_y = INTERPOLATION(prew->second.tar_y, next->second.tar_y, dt);
		state.tar_z = INTERPOLATION(prew->second.tar_z, next->second.tar_z, dt);
		state.cr_x = INTERPOLATION(prew->second.cr_x, next->second.cr_x, dt);
		state.cr_y = INTERPOLATION(prew->second.cr_y, next->second.cr_y, dt);
		state.cr_z = INTERPOLATION(prew->second.cr_z, next->second.cr_z, dt);
		state.fov = INTERPOLATION(prew->second.fov, next->second.fov, dt);

//		printf("Dist: %f\n", state.distance);

#undef INTERPOLATION

	}

//	printf("XYZ: %f %f %f\n", state.tar_x, state.tar_y, state.tar_z);
//	printf("RXYZ: %d %f %f %f\n", delta, state.cr_z, state.cr_z, state.cr_z);
//	printf("Dist: %f\n", state.distance);

}

static bool c_cmp(std::pair<Uint32, CameraState>& a, std::pair<Uint32, CameraState>& b) {
	 return a.first < b.first;
}

static void c_sort(std::map<Uint32, CameraState>& M) {
	std::vector<std::pair<Uint32, CameraState>> A;
	for (auto& it : M) {
		A.push_back(it);
	}
	std::sort(A.begin(), A.end(), c_cmp);
	M.clear();
	for (auto& it : A) {
		M[it.first] = it.second;
	}
}

void CameraAnimationTrack::AddKeyFrame(Uint32 time, CameraState frame) {
	key_frames[time] = frame;

	// TODO: remove this code
	c_sort(key_frames);
}

CameraState CameraAnimationTrack::GetValue() {
	return state;
}


Animation::Animation() {}
Animation::~Animation() {}

void Animation::AddTrack(AnimationTrack* track) {
	RG_ASSERT_MSG(track, "NULL track!");
	tracks.push_back(track);
}

void Animation::Update() {
	animation_time += Engine::GetDeltaTime() * animation_fps * animation_speed;

	for (Uint32 i = 0; i < tracks.size(); ++i) {
		tracks[i]->Update();
	}
}

void Animation::Reset() {
	animation_time = 0;
}

void Animation::SetLastFrame(Uint32 lf) {
	last_frame = lf;
}

Uint32 Animation::GetLastFrame() {
	return last_frame;
}

double Animation::GetAnimationTime() {
	return animation_time;
}

double Animation::GetAnimationFramerate() {
	return animation_fps;
}

void Animation::SetAnimationFramerate(double fps) {
	animation_fps = fps;
}

double Animation::GetAnimationSpeed() {
	return animation_speed;
}

void Animation::SetAnimationSpeed(double speed) {
	animation_speed = speed;
}

//static Uint32 skipped = 0;

AnimationTrack* Animation::GetTrack(String name) {
	for (Uint32 i = 0; i < tracks.size(); ++i) {
		AnimationTrack* track = tracks[i];
		if(Engine::Helper::streql(track->GetName(), name)) {
			return track;
		}
//		printf("~~~ C %s %s\n", track->GetName(), name);
	}
//	printf("Skipping %d %d %s\n", skipped, tracks.size(), name);
//	skipped++;

//	Engine::Quit();
	return NULL;
}

AnimationTrack* Animation::GetTrack(Uint32 id) {
	return tracks[id];
}

