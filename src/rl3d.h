/*
 * cl_renderer_line3d.h
 *
 *  Created on: Nov 30, 2021
 *      Author: alex9932
 */

#ifndef RL3D_H_
#define RL3D_H_

#include <engine/core/math/matrix.h>
#include <engine/core/math/vector.h>
#include <engine/level/camera.h>

typedef struct cl_vertex3d_t {
	Engine::Math::vec3 vertex;
	Engine::Math::vec4 color;
} cl_vertex3d_t;

typedef void (*cl_r3d_drawfunc)(double);

void cl_rl3d_init();
void cl_rl3d_destroy();
void cl_rl3d_doRender(double dt, cl_r3d_drawfunc rf, Engine::Camera* cam);

void cl_rl3d_applyMatrix(Engine::Math::mat4* matrix);
void cl_rl3d_begin();
void cl_rl3d_end();
void cl_rl3d_vertex(const cl_vertex3d_t& vertex);

#endif /* RL3D_H_ */
