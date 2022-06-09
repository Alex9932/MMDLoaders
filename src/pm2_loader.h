/*
 * pm2_loader.h
 *
 *  Created on: Apr 20, 2022
 *      Author: alex9932
 */

#ifndef PM2_LOADER_H_
#define PM2_LOADER_H_

#include <engine/core/geometry.h>

class PM2Loader: public Engine::GeometryLoader {
	public:
		PM2Loader(Engine::Allocator* alloc): GeometryLoader(alloc) {}
		virtual ~PM2Loader() {}
		Engine::Geometry* LoadGeometry(String path);
};

#endif /* PM2_LOADER_H_ */
