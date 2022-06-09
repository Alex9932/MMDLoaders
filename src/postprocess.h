/*
 * postprocess.h
 *
 *  Created on: Apr 3, 2022
 *      Author: alex9932
 */

#ifndef POSTPROCESS_H_
#define POSTPROCESS_H_

#include <GL/glew.h>
#include <engine/level/camera.h>
#include <engine/render/shader.h>

class PPEffect {
	private:
		Engine::Render::Shader* shader;
		GLuint framebuffer;
		GLuint input[8];
		Uint32 inputs;
		GLuint output;

	public:
		PPEffect(String program, Uint32 inputs);
		virtual ~PPEffect();
		virtual void Render();
		virtual void ApplyUniforms(Engine::Render::Shader* shader) {}
		RG_FORCE_INLINE GLuint GetOutput() { return this->output; }
		RG_FORCE_INLINE void   SetInput(Uint32 i, GLuint texture) { this->input[i] = texture; }

};

class PPLightEffect : public PPEffect {
	public:
		PPLightEffect(String program, Uint32 inputs) : PPEffect(program, inputs) {}
		virtual ~PPLightEffect() {}
		virtual void ApplyUniforms(Engine::Render::Shader* shader);

};

void PPInitialize();
void PPDestroy();
void PPDoPostProcess(Engine::Camera* camera);
GLuint PPGetOutput();


#endif /* POSTPROCESS_H_ */
