/*
 * camera.cpp
 *
 *  Created on: Apr 27, 2022
 *      Author: alex9932
 */

#include "camera.h"

#include <SDL2/SDL_scancode.h>
#include <engine/core/input.h>
#include <engine/core/math/math.h>
#include <engine/core/math/matrix.h>
#include <engine/core/math/vector.h>

static void math_getForward(Engine::Math::vec3* vec, const Engine::Math::vec3& angles) {
	vec->x = (float) SDL_sin(angles.y);
	vec->y = (float)-SDL_tan(angles.x);
	vec->z = (float)-SDL_cos(angles.y);
	Engine::Math::vec3_normalize(vec, *vec);

//	Engine::Math::vec4 vector = {0, 0, 1, 0};
//	Engine::Math::vec4 dst;
//	Engine::Math::mat4 rotation;
//	Engine::Math::mat4_rotate(&rotation, angles);
//	Engine::Math::mat4_mul(&dst, vector, rotation);
//	vec->x = dst.x;
//	vec->y = dst.y;
//	vec->z = dst.z;
//	vec->x = SDL_sin(angles.x) * SDL_cos(angles.y);
//	vec->y = SDL_sin(angles.x) * SDL_sin(angles.y);
//	vec->z = SDL_cos(angles.y);
//	Engine::Math::vec3_normalize(vec, *vec);
}

SmoothCamera::SmoothCamera(const Engine::Math::vec3& pos, float fov, float near, float far, float aspect)
		: Camera(pos, fov, near, far, aspect) {
	SetSpeed({1, 0.2, 3});

	velocity.x = 0;
	velocity.y = 0;
	velocity.z = 0;
	rotation.x = 0;
	rotation.y = 0;
	rotation.z = 0;

	delta_mouse.x = 0;
	delta_mouse.y = 0;
	delta_pos.x = 0;
	delta_pos.y = 0;
	delta_pos.z = 0;
	delta_rot = 0;
}

SmoothCamera::~SmoothCamera() {
}

void SmoothCamera::Update() {
	if(Engine::Input::IsButtonDown(3)) {
		delta_mouse.x += Engine::Input::GetMouseDY() * 0.01;
		delta_mouse.y += Engine::Input::GetMouseDX() * 0.01;
	}

	rotation.x += delta_mouse.x;
	rotation.y += delta_mouse.y;
	Engine::Math::vec2_mul(&delta_mouse, delta_mouse, 0.87f);

	double delta = Engine::GetDeltaTime();

	Engine::Math::vec3 fwd = {0, 0, -1};
	Engine::Math::vec3 vec;
	Engine::Math::vec3 up = {0, 1, 0};
	math_getForward(&fwd, {rotation.x, rotation.y, 0});
//	math_getForward(&up, {rotation.y, rotation.x + RG_HALF_PI});
	Engine::Math::vec3_cross(&vec, fwd, up);
	Engine::Math::vec3_normalize(&vec, vec);

	float speed = speed_default;

	if(Engine::Input::IsKeyDown(SDL_SCANCODE_LCTRL)) {
		speed = speed_fast;
	}
	if(Engine::Input::IsKeyDown(SDL_SCANCODE_LSHIFT)) {
		speed = speed_slow;
	}

	if(Engine::Input::IsKeyDown(SDL_SCANCODE_W)) {
		delta_pos.x += fwd.x * speed * delta;
		delta_pos.y += fwd.y * speed * delta;
		delta_pos.z += fwd.z * speed * delta;
	}
	if(Engine::Input::IsKeyDown(SDL_SCANCODE_S)) {
		delta_pos.x -= fwd.x * speed * delta;
		delta_pos.y -= fwd.y * speed * delta;
		delta_pos.z -= fwd.z * speed * delta;
	}
	if(Engine::Input::IsKeyDown(SDL_SCANCODE_A)) {
		delta_pos.x -= vec.x * speed * delta;
		delta_pos.y -= vec.y * speed * delta;
		delta_pos.z -= vec.z * speed * delta;
		delta_rot -= delta * 0.4;
	}
	if(Engine::Input::IsKeyDown(SDL_SCANCODE_D)) {
		delta_pos.x += vec.x * speed * delta;
		delta_pos.y += vec.y * speed * delta;
		delta_pos.z += vec.z * speed * delta;
		delta_rot += delta * 0.4;
	}
	Engine::Math::vec3_add(&position, position, delta_pos);
	Engine::Math::vec3_mul(&delta_pos, delta_pos, 0.91f);

//	float friction = 0.9f;
//	if(delta_pos.x > 0) { delta_pos.x -= friction * delta; }
//	if(delta_pos.y > 0) { delta_pos.y -= friction * delta; }
//	if(delta_pos.z > 0) { delta_pos.z -= friction * delta; }
//	if(delta_pos.x < 0) { delta_pos.x += friction * delta; }
//	if(delta_pos.y < 0) { delta_pos.y += friction * delta; }
//	if(delta_pos.z < 0) { delta_pos.z += friction * delta; }


	rotation.z = -delta_rot;
//	delta_rot -= delta_rot * delta * 0.91;
	delta_rot *= 0.91;

	Engine::Math::mat4_view(&transform, position, rotation);
}
