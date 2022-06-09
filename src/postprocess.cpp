/*
 * postprocess.cpp
 *
 *  Created on: Apr 3, 2022
 *      Author: alex9932
 */

#include "postprocess.h"
#include "gbuffer.h"
//#include "GLOBALS.h"

#include <engine/render/shader.h>
#include <engine/render/window.h>
#include <engine/core/math/math.h>

#define PP_VERTEX_PROGRAM "platform/shaders/pp/main.vs"

static GLuint quad;
static GLuint quad_vbo;
static GLuint quad_ebo;

PPEffect::PPEffect(String program, Uint32 inputs) {
	Engine::Math::vec2 size;
	Engine::Window::GetWindowSize(&size);

	this->inputs = inputs;
	glGenFramebuffers(1, &this->framebuffer);
	glGenTextures(1, &this->output);
	glBindTexture(GL_TEXTURE_2D, this->output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->output, 0);
	GLenum color_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, color_buffers);
	this->shader = new Engine::Render::Shader(PP_VERTEX_PROGRAM, program, NULL);
}

PPEffect::~PPEffect() {
	glDeleteFramebuffers(1, &this->framebuffer);
	glDeleteTextures(1, &this->output);
	delete this->shader;
}

void PPEffect::Render() {
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	glClear(GL_COLOR_BUFFER_BIT);

	this->shader->Start();

	// Setup Textures
	this->shader->SetInt(this->shader->GetUniformLocation("t_unit0"), 0);
	this->shader->SetInt(this->shader->GetUniformLocation("t_unit1"), 1);
	this->shader->SetInt(this->shader->GetUniformLocation("t_unit2"), 2);
	this->shader->SetInt(this->shader->GetUniformLocation("t_unit3"), 3);
	this->shader->SetInt(this->shader->GetUniformLocation("t_unit4"), 4);
	this->shader->SetInt(this->shader->GetUniformLocation("t_unit5"), 5);
	this->shader->SetInt(this->shader->GetUniformLocation("t_unit6"), 6);
	this->shader->SetInt(this->shader->GetUniformLocation("t_unit7"), 7);

	for (Uint32 i = 0; i < this->inputs; ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, this->input[i]);
	}

	this->ApplyUniforms(this->shader);

	// Render
	glBindVertexArray(quad);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	this->shader->Stop();
}

Engine::Camera* s_camera = NULL; // Initialized in PPDoPostProcess function

void PPLightEffect::ApplyUniforms(Engine::Render::Shader* shader) {
	Engine::Math::mat4 invProj;
	Engine::Math::mat4 invView;
//	Engine::Math::mat4 invViewProj;

	Engine::Math::mat4_inverse(&invProj, &s_camera->GetProjection());
	Engine::Math::mat4_inverse(&invView, &s_camera->GetTransform());
//	Engine::Math::mat4_mul(&invViewProj, invView, invProj);

//	Engine::Math::mat4_mul(&viewProj, s_camera->GetProjection(), s_camera->GetTransform());
//	Engine::Math::mat4_inverse(&invViewProj, &viewProj);

	shader->SetMat4(shader->GetUniformLocation("invProj"), &invProj);
	shader->SetMat4(shader->GetUniformLocation("invView"), &invView);
	shader->SetVec3(shader->GetUniformLocation("camera"), s_camera->GetPosition());
	shader->SetFloat(shader->GetUniformLocation("far"), s_camera->GetFar());
	shader->SetFloat(shader->GetUniformLocation("near"), s_camera->GetNear());
}


//static GLuint output;
//static GLuint fb;
//
//static GLuint edge_output;
//static GLuint edge_fb;
//
//static GLuint out;
//static GLuint out_fb;
//
//static Engine::Render::Shader* pp_shader;
//static Engine::Render::Shader* pp_edge;
//static Engine::Render::Shader* pp_out;

static float vertices[] = { 1,  1, 1, -1, -1, -1, -1,  1 };
static Uint16 indices[] = { 0, 1, 3, 1, 2, 3 };

static PPEffect* e_edge;
static PPLightEffect* e_light;

void PPInitialize() {
//	pp_shader = new Engine::Render::Shader("platform/shaders/pp/main.vs", "platform/shaders/pp/main.fs", NULL);
//	pp_edge = new Engine::Render::Shader("platform/shaders/pp/main.vs", "platform/shaders/pp/edge.fs", NULL);
//	pp_out = new Engine::Render::Shader("platform/shaders/pp/main.vs", "platform/shaders/pp/mix.fs", NULL);

	glGenVertexArrays(1, &quad);
	glBindVertexArray(quad);
	glGenBuffers(1, &quad_vbo);
	glGenBuffers(1, &quad_ebo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &quad_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



	e_edge = new PPEffect("platform/shaders/pp/edge.fs", 1);
//	e_edge->SetInput(0, e_light->GetOutput());
	e_edge->SetInput(0, GBufferGetAmbient());

	e_light = new PPLightEffect("platform/shaders/pp/light.fs", 3);
//	e_light->SetInput(0, GBufferGetAmbient());
	e_light->SetInput(0, e_edge->GetOutput());
	e_light->SetInput(1, GBufferGetNormal());
	e_light->SetInput(2, GBufferGetDepth());

//	Engine::Math::vec2 size;
//	Engine::Window::GetWindowSize(&size);
//
//	glGenFramebuffers(1, &fb);
//	glGenTextures(1, &output);
//	glBindTexture(GL_TEXTURE_2D, output);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glBindFramebuffer(GL_FRAMEBUFFER, fb);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);
//	GLenum color_buffers[] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, color_buffers);
//
//	glGenFramebuffers(1, &edge_fb);
//	glGenTextures(1, &edge_output);
//	glBindTexture(GL_TEXTURE_2D, edge_output);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glBindFramebuffer(GL_FRAMEBUFFER, edge_fb);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, edge_output, 0);
//	glDrawBuffers(1, color_buffers);
//
//	glGenFramebuffers(1, &out_fb);
//	glGenTextures(1, &out);
//	glBindTexture(GL_TEXTURE_2D, out);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glBindFramebuffer(GL_FRAMEBUFFER, out_fb);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out, 0);
//	glDrawBuffers(1, color_buffers);
}

void PPDestroy() {
//	glDeleteFramebuffers(1, &fb);
//	glDeleteTextures(1, &output);
//	glDeleteFramebuffers(1, &edge_fb);
//	glDeleteTextures(1, &edge_output);

	delete e_edge;
	delete e_light;
}

void PPDoPostProcess(Engine::Camera* camera) {

	s_camera = camera;

	Engine::Math::vec2 size;
	Engine::Window::GetWindowSize(&size);
	glViewport(0, 0, size.x, size.y);

	e_edge->Render();
	e_light->Render();

//	glBindFramebuffer(GL_FRAMEBUFFER, fb);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	pp_shader->Start();
//	pp_shader->SetInt(pp_shader->GetUniformLocation("t_unit0"), 0);
//	pp_shader->SetInt(pp_shader->GetUniformLocation("t_unit1"), 1);
//	pp_shader->SetInt(pp_shader->GetUniformLocation("t_unit2"), 2);
//	pp_shader->SetInt(pp_shader->GetUniformLocation("t_unit3"), 3);
//	pp_shader->SetInt(pp_shader->GetUniformLocation("t_unit4"), 4);
//	pp_shader->SetInt(pp_shader->GetUniformLocation("t_unit5"), 5);
//	pp_shader->SetInt(pp_shader->GetUniformLocation("toon"), toon);
//	pp_shader->SetFloat(pp_shader->GetUniformLocation("levels"), toon_levels);
//	pp_shader->SetVec3(pp_shader->GetUniformLocation("sun_pos"), sun_pos);
//	pp_shader->SetVec3(pp_shader->GetUniformLocation("camera_pos"), camera->GetPosition());
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, GBufferGetAmbient());
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, GBufferGetDiffuse());
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, GBufferGetSpecular());
//	glActiveTexture(GL_TEXTURE3);
//	glBindTexture(GL_TEXTURE_2D, GBufferGetPosition());
//	glActiveTexture(GL_TEXTURE4);
//	glBindTexture(GL_TEXTURE_2D, GBufferGetNormal());
//	glActiveTexture(GL_TEXTURE5);
//	glBindTexture(GL_TEXTURE_2D, GBufferGetDepth());
//	glBindVertexArray(quad);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, edge_fb);
//	glClear(GL_COLOR_BUFFER_BIT);
//	pp_edge->Start();
//	pp_edge->SetInt(pp_edge->GetUniformLocation("t_unit0"), 0);
//	glActiveTexture(GL_TEXTURE0);
////	glBindTexture(GL_TEXTURE_2D, output);
//	glBindTexture(GL_TEXTURE_2D, GBufferGetAmbient());
//	glBindVertexArray(quad);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, out_fb);
//	glClear(GL_COLOR_BUFFER_BIT);
//	pp_out->Start();
//	pp_out->SetInt(pp_out->GetUniformLocation("t_unit0"), 0);
//	pp_out->SetInt(pp_out->GetUniformLocation("t_unit1"), 1);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, output);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, edge_output);
//	glBindVertexArray(quad);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

GLuint PPGetOutput() {
//	return output;
//	return out;
//	return edge_output;

	return e_light->GetOutput();
}
