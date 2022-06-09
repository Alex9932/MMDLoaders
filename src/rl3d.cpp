/*
 * cl_renderer_line3d.cpp
 *
 *  Created on: Nov 30, 2021
 *      Author: alex9932
 */

#include "rl3d.h"

#include <engine/render/shader.h>

#define RG_L3D_MAX_VERTECES 2048

static Engine::Render::Shader* l3d_shader;
static GLuint vao;
static GLuint vbo;

static Engine::Math::mat4 g_model;
static GLuint g_vertex;

static cl_vertex3d_t* vertex_buffer;

void cl_rl3d_init() {

	vertex_buffer = (cl_vertex3d_t*)malloc(sizeof(cl_vertex3d_t) * RG_L3D_MAX_VERTECES);

	glLineWidth(1.2);

	l3d_shader = new Engine::Render::Shader("platform/shaders/l3d.vs", "platform/shaders/l3d.fs", NULL);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cl_vertex3d_t) * RG_L3D_MAX_VERTECES, NULL, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cl_vertex3d_t), (GLvoid*)(sizeof(float) * 0)); // vertex
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(cl_vertex3d_t), (GLvoid*)(sizeof(float) * 3)); // color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void cl_rl3d_destroy() {
	delete l3d_shader;
	free(vertex_buffer);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void cl_rl3d_doRender(double dt, cl_r3d_drawfunc rf, Engine::Camera* cam) {
	glDisable(GL_DEPTH_TEST);
	l3d_shader->Start();
	l3d_shader->SetMat4(l3d_shader->GetUniformLocation("proj"), &cam->GetProjection());
	l3d_shader->SetMat4(l3d_shader->GetUniformLocation("view"), &cam->GetTransform());
	rf(dt);
}

void cl_rl3d_begin() {
	g_vertex = 0;
	mat4_identity(&g_model);
	glBindVertexArray(vao);
}

void cl_rl3d_applyMatrix(Engine::Math::mat4* matrix) {
	l3d_shader->SetMat4(l3d_shader->GetUniformLocation("model"), matrix);
}

void cl_rl3d_end() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cl_vertex3d_t) * RG_L3D_MAX_VERTECES, vertex_buffer);
	glDrawArrays(GL_LINES, 0, g_vertex);
}

void cl_rl3d_vertex(const cl_vertex3d_t& vertex) {
	if(g_vertex >= RG_L3D_MAX_VERTECES) {
		return;
	}
	vertex_buffer[g_vertex] = vertex;
	g_vertex++;
}

