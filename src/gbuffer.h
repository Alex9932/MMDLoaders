/*
 * gbuffer.h
 *
 *  Created on: Apr 3, 2022
 *      Author: alex9932
 */

#ifndef GBUFFER_H_
#define GBUFFER_H_

#include <GL/glew.h>

void GBufferInitialize();
void GBufferDestroy();

GLuint GBufferGetBuffer();

GLuint GBufferGetAmbient();
//GLuint GBufferGetDiffuse();
//GLuint GBufferGetSpecular();
//GLuint GBufferGetPosition();
GLuint GBufferGetNormal();
GLuint GBufferGetDepth();

#endif /* GBUFFER_H_ */
