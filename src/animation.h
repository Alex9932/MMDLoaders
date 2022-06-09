/*
 * animation.h
 *
 *  Created on: Apr 7, 2022
 *      Author: alex9932
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <engine/engine.h>
#include <SDL2/SDL.h>
#include <vector>
#include <map>

typedef struct BoneState {
	float px;
	float py;
	float pz;
	float rx;
	float ry;
	float rz;
	float rw;
	char interpolation[64];
} BoneState;

typedef struct CameraState {
	float distance;
	float tar_x;
	float tar_y;
	float tar_z;
	float cr_x;
	float cr_y;
	float cr_z;
	float fov;
	char interpolation[24];
} CameraState;

class AnimationTrack {
	protected:
		class Animation* animation;
		char name[32];
		Uint32 prew_frame = 0;
		Uint32 next_frame = 0;

	public:
		AnimationTrack(class Animation* anim, String n) {
			this->animation = anim;
			SDL_memset(this->name, 0, 32);
			SDL_memcpy(this->name, n, SDL_strlen(n));
		}
		virtual ~AnimationTrack() {}
		virtual void Update() {}
		String GetName() { return name; }
};

class BoneAnimationTrack: public AnimationTrack {
	private:
		std::map<Uint32, BoneState> key_frames;
		BoneState state;
		Uint32 id;

	public:
		BoneAnimationTrack(class Animation* anim, String name, Uint32 id);
		virtual ~BoneAnimationTrack();
		virtual void Update();
		void AddKeyFrame(Uint32 time, BoneState frame);
		BoneState GetValue();
		std::map<Uint32, BoneState>& GetKeyFrames();
};

class FaceAnimationTrack: public AnimationTrack {
	private:
		std::map<Uint32, float> key_frames;
		float weight = 0;

	public:
		FaceAnimationTrack(class Animation* anim, String name);
		virtual ~FaceAnimationTrack();
		virtual void Update();
		void AddKeyFrame(Uint32 time, float weight);
		float GetValue();
};

class CameraAnimationTrack: public AnimationTrack {
	private:
		std::map<Uint32, CameraState> key_frames;
		CameraState state;

	public:
		CameraAnimationTrack(class Animation* anim, String name);
		virtual ~CameraAnimationTrack();
		virtual void Update();
		void AddKeyFrame(Uint32 time, CameraState frame);
		CameraState GetValue();
};

class Animation {
	private:
		std::vector<AnimationTrack*> tracks;
		double animation_time = 0;
		double animation_fps = 30; // vmd animation in 30 fps
		double animation_speed = 1;
		Uint32 last_frame = 0;

	public:
		Animation();
		virtual ~Animation();
		void AddTrack(AnimationTrack* track);
		void Update();
		void Reset();
		void SetLastFrame(Uint32 lf);
		Uint32 GetLastFrame();
		double GetAnimationTime();

		double GetAnimationFramerate();
		void SetAnimationFramerate(double fps);
		double GetAnimationSpeed();
		void SetAnimationSpeed(double speed);

		AnimationTrack* GetTrack(String name);
		AnimationTrack* GetTrack(Uint32 id);
};


#endif /* ANIMATION_H_ */
