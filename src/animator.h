/*
 * animator.h
 *
 *  Created on: Apr 17, 2022
 *      Author: alex9932
 */

#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#include "animation.h"
#include <engine/render/model.h>
#include "mmd/pmd.h"


class Animator {
	private:
		Animation* animation;

	public:
		Animator();
		virtual ~Animator();

		void SetAnimation(Animation* anim);
		Animation* GetAnimation();

//		void ApplayAnimation(Engine::Render::RiggedModel* model);
		void ApplayAnimation(Engine::Skeleton* skeleton);
		void ApplayAnimation(Engine::Skeleton* skeleton, Engine::Bone* state);
		void ApplayFace(Engine::Render::RiggedModel* model, pmd_morph* morphs);

};


#endif /* ANIMATOR_H_ */
