/*
 * pm2_loader.cpp
 *
 *  Created on: Apr 20, 2022
 *      Author: alex9932
 */

#include "pm2_loader.h"
#include "pm2.h"
#include <engine/core/filesystem.h>

//#define Z_FLIPPED

Engine::Geometry* PM2Loader::LoadGeometry(String path) {

	rgLogInfo(RG_LOG_SYSTEM, "Loading pm2: %s", path);
	Engine::Filesystem::Resource* res = Engine::Filesystem::GetResource(path);
	size_t pointer = 0;
	char* data = (char*)res->data;
	PM2_Header header = *(PM2_Header*)&data[pointer];
	pointer += sizeof(PM2_Header);
//			rgLogInfo(RG_LOG_GAME, "Materials in model: %d", header.materials);


	Engine::Geometry* geom = (Engine::Geometry*)malloc(sizeof(Engine::Geometry));

	geom->vertex_count = header.vertices;
	geom->indices_total = header.indices;
	geom->meshes = header.materials;

	geom->vertices = (Engine::Render::Vertex*)malloc(sizeof(Engine::Render::Vertex) * geom->vertex_count);
	geom->indices = (Uint32*)malloc(sizeof(Uint32) * geom->indices_total);
	geom->materials = (Engine::Render::Material**)malloc(sizeof(Engine::Render::Material*) * geom->meshes);
	geom->indices_count = (Uint32*)malloc(sizeof(Uint32) * geom->meshes);


	// Read materials

	Engine::Render::Material* mat = NULL;//new Engine::Render::Material("stone_path");

	for (Uint32 i = 0; i < geom->meshes; ++i) {

		// PM2_string
		Uint32 len = *(Uint32*)&data[pointer];
		pointer += sizeof(Uint32);
		const char* str = (char*)&data[pointer];
		pointer += len;
		Uint32 indices_count = *(Uint32*)&data[pointer];
		pointer += sizeof(Uint32);
		PM2_Color* diffuse = (PM2_Color*)&data[pointer];
		pointer += sizeof(PM2_Color);
		PM2_Color* ambient = (PM2_Color*)&data[pointer];
		pointer += sizeof(PM2_Color);
//				rgLogInfo(RG_LOG_GAME, "Materials: %d, %s", len, str);

		char b[128];
		memset(b, 0, 128);
		memcpy(b, str, len);
		mat = new Engine::Render::Material(b);
//		mat->SetColor({diffuse->r, diffuse->g, diffuse->b, 1});
//		mat->SetAmbient({ambient->r, ambient->g, ambient->b});
		mat->SetColor({ambient->r, ambient->g, ambient->b, 1});
		mat->SetAmbient({diffuse->r, diffuse->g, diffuse->b});

		geom->indices_count[i] = indices_count;
		geom->materials[i] = mat;
	}

	// Read vertices

	PM2_Vertex* vtx = (PM2_Vertex*)&data[pointer];
	pointer += sizeof(PM2_Vertex) * geom->vertex_count;
//			rgLogInfo(RG_LOG_GAME, "Vertices: %d 0x%x 0x%x 0x%x", geom->vertex_count, sizeof(PM2_Vertex), geom->vertex_count, sizeof(PM2_Vertex) * geom->vertex_count);

	for (Uint32 i = 0; i < geom->vertex_count; ++i) {
#ifdef Z_FLIPPED

		geom->vertices[i].vertex.x = vtx[i].position.x;
		geom->vertices[i].vertex.y = vtx[i].position.z;
		geom->vertices[i].vertex.z = vtx[i].position.y;
		geom->vertices[i].normal.x = vtx[i].normal.x;
		geom->vertices[i].normal.y = vtx[i].normal.z;
		geom->vertices[i].normal.z = vtx[i].normal.y;
		geom->vertices[i].tangent.x = vtx[i].tangent.x;
		geom->vertices[i].tangent.y = vtx[i].tangent.z;
		geom->vertices[i].tangent.z = vtx[i].tangent.y;
		geom->vertices[i].t_coord.x = vtx[i].texture.x;
		geom->vertices[i].t_coord.y = 1 - vtx[i].texture.y;

#else
		geom->vertices[i].vertex.x = vtx[i].position.x;
		geom->vertices[i].vertex.y = vtx[i].position.y;
		geom->vertices[i].vertex.z = vtx[i].position.z;
		geom->vertices[i].normal.x = vtx[i].normal.x;
		geom->vertices[i].normal.y = vtx[i].normal.y;
		geom->vertices[i].normal.z = vtx[i].normal.z;
		geom->vertices[i].tangent.x = vtx[i].tangent.x;
		geom->vertices[i].tangent.y = vtx[i].tangent.y;
		geom->vertices[i].tangent.z = vtx[i].tangent.z;
		geom->vertices[i].t_coord.x = vtx[i].texture.x;
		geom->vertices[i].t_coord.y = vtx[i].texture.y;
#endif
	}


	// Read indices
	for (Uint32 i = 0; i < geom->indices_total; ++i) {

		if(res->length <= pointer) {
			rgLogInfo(RG_LOG_GAME, "ERROR: 0x%x 0x%x 0x%x 0x%x", res->data, res->length, res->length, data);
			break;
		}

		Uint32 index = *(Uint32*)&data[pointer];
		pointer += sizeof(Uint32);

		geom->indices[i] = index;

	}

	return geom;
}

