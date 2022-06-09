/*
 * rg_mmd_loader.h
 *
 *  Created on: Mar 31, 2022
 *      Author: alex9932
 */

#ifndef RG_MMD_LOADER_H_
#define RG_MMD_LOADER_H_

#include <engine/core/allocator.h>
#include <engine/core/loader.h>
#include <engine/core/skeleton.h>
#include <engine/core/geometry.h>
#include "vmd.h"
#include "pmd.h"
//#include "pmx.h"
#include "../animation.h"

class MMD_PMD_GeometryLoader: public Engine::RiggedGeometryLoader {
	public:
		MMD_PMD_GeometryLoader(Engine::Allocator* allocator): RiggedGeometryLoader(allocator) {}
		virtual ~MMD_PMD_GeometryLoader() {}
		virtual Engine::RiggedGeometry* LoadGeometry(String path);
		Engine::RiggedGeometry* LoadGeometry(String path, pmd_file* pmd);
		Engine::Skeleton* LoadSkeleton(pmd_file* pmd);
};

class MMD_PMX_GeometryLoader: public Engine::RiggedGeometryLoader {
	public:
		MMD_PMX_GeometryLoader(Engine::Allocator* allocator): RiggedGeometryLoader(allocator) {}
		virtual ~MMD_PMX_GeometryLoader() {}
		virtual Engine::RiggedGeometry* LoadGeometry(String path);
		Engine::RiggedGeometry* LoadGeometry(String path, pmx_file* pmx);
		Engine::Skeleton* LoadSkeleton(pmx_file* pmx);
};

class MMD_VMD_AnimationLoader {
	public:
		MMD_VMD_AnimationLoader() {}
		virtual ~MMD_VMD_AnimationLoader() {}
		Animation* LoadCameraAnimation(vmd_file* vmd);
		Animation* LoadAnimation(pmd_file* pmd, vmd_file* vmd, Engine::Skeleton* skeleton);
};



#endif /* RG_MMD_LOADER_H_ */
