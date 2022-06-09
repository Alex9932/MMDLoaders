/*
 * pm2.h
 *
 *  Created on: Mar 9, 2022
 *      Author: alex9932
 */

#ifndef PM2_H_
#define PM2_H_

#include <SDL2/SDL.h>

#define PM2_FLAG_IS_RIGGED        0b00000001
#define PM2_FLAG_EXTENDED_INDICES 0b00000010

typedef struct PM2_Header {
	char sig[4];
	Uint32 materials;
	Uint32 vertices;
	Uint32 indices;
	Uint8 flags;
	Uint8 offset[3];
} PM2_Header;

typedef struct PM2_Color {
	float r;
	float g;
	float b;
	float a;
} PM2_Color;

typedef struct PM2_Vec3 {
	float x;
	float y;
	float z;
} PM2_Vec3;

typedef struct PM2_Vec2 {
	float x;
	float y;
} PM2_Vec2;

typedef struct PM2_String {
	Uint32 len;
	char* str;
} PM2_String;

typedef struct PM2_Material {
	PM2_String name;
	Uint32 indices;
	PM2_Color diffuse;
	PM2_Color ambient;
} PM2_Material;

typedef struct PM2_Vertex {
	PM2_Vec3 position;
	PM2_Vec3 normal;
	PM2_Vec3 tangent;
	PM2_Vec2 texture;
} PM2_Vertex;

typedef struct PM2_File {
	PM2_Header header;
	PM2_Material* materials;
	PM2_Vertex* vertices;
	void* indices;
} PM2_File;


#endif /* PM2_H_ */
