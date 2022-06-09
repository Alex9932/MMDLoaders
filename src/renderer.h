/*
 * renderer.h
 *
 *  Created on: May 10, 2022
 *      Author: alex9932
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include <SDL2/SDL.h>
#include <engine/core/math/math.h>
#include <engine/level/camera.h>

#include <GL/glew.h>

extern GLuint O_BUFFER;

extern Uint32 box_model_id;
extern Uint32 sphere_model_id;
extern Uint32 cylinder_model_id;

void InitializeRenderer();
void DestroyRenderer();

Engine::Camera* GetCamera();

void RenderScene(Engine::Math::vec4& camera_angles, bool can_control, bool r_wireframe);


#endif /* RENDERER_H_ */
