/*
 * gbuffer.cpp
 *
 *  Created on: Apr 3, 2022
 *      Author: alex9932
 */

#include "gbuffer.h"

#include <engine/render/window.h>
#include <engine/core/math/math.h>
#include <engine/core/logger.h>

static GLuint framebuffer;
static GLuint a_ambient;
//static GLuint a_diffuse;
//static GLuint a_specular;
//static GLuint a_position;
static GLuint a_normal;
static GLuint a_depth;

static void RemakeBuffer() {
	Engine::Math::vec2 size;
	Engine::Window::GetWindowSize(&size);

	//glEnable(GL_FRAMEBUFFER_SRGB);

	glGenFramebuffers(1, &framebuffer);

	glGenTextures(1, &a_ambient);
//	glGenTextures(1, &a_diffuse);
//	glGenTextures(1, &a_specular);
//	glGenTextures(1, &a_position);
	glGenTextures(1, &a_normal);
	glGenTextures(1, &a_depth);

	glBindTexture(GL_TEXTURE_2D, a_ambient);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	glBindTexture(GL_TEXTURE_2D, a_diffuse);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	glBindTexture(GL_TEXTURE_2D, a_specular);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	glBindTexture(GL_TEXTURE_2D, a_position);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, a_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, a_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, a_ambient, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, a_diffuse, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, a_specular, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, a_position, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, a_normal, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, a_depth, 0);

	GLenum color_buffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
//			GL_COLOR_ATTACHMENT2,
//			GL_COLOR_ATTACHMENT3,
//			GL_COLOR_ATTACHMENT4
	};
	glDrawBuffers(2, color_buffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		rgLogInfo(RG_LOG_RENDER, "GBUFFER ERROR! %s", glewGetString(status));
	}

}

void GBufferInitialize() {
	RemakeBuffer();
}

void GBufferDestroy() {
	glDeleteTextures(1, &a_ambient);
//	glDeleteTextures(1, &a_diffuse);
//	glDeleteTextures(1, &a_specular);
//	glDeleteTextures(1, &a_position);
	glDeleteTextures(1, &a_normal);
	glDeleteTextures(1, &a_depth);
	glDeleteFramebuffers(1, &framebuffer);
}

GLuint GBufferGetBuffer() {
	return framebuffer;
}
//
GLuint GBufferGetAmbient() {
	return a_ambient;
}
//
//GLuint GBufferGetDiffuse() {
//	return a_diffuse;
//}
//
//GLuint GBufferGetSpecular() {
//	return a_specular;
//}
//
//GLuint GBufferGetPosition() {
//	return a_position;
//}

GLuint GBufferGetNormal() {
	return a_normal;
}

GLuint GBufferGetDepth() {
	return a_depth;
}

