/*
 * camera.h
 *
 *  Created on: Apr 27, 2022
 *      Author: alex9932
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <engine/level/camera.h>

class SmoothCamera : public Engine::Camera {

	private:
		Engine::Math::vec2 delta_mouse;
		Engine::Math::vec3 delta_pos;
		float delta_rot;


	public:
		SmoothCamera(const Engine::Math::vec3& pos, float fov, float near, float far, float aspect);
		virtual ~SmoothCamera();
		virtual void Update();
};

#endif /* CAMERA_H_ */
