#pragma once
#ifndef __EULERIAN_2D_RENDERER_H__
#define __EULERIAN_2D_RENDERER_H__


#include <stb_image.h>
#include "Eulerian/include/MACGrid2d.h"
#include <glfw3.h>
#include "Configure.h"
#include "glm/glm.hpp"
#include "Shader.h"

namespace FluidSimulation {
	namespace Eulerian2d {

		class Renderer {
		public:
			Renderer();

			void draw(MACGrid2d& mGrid);

			void destroy();

			void loadTexture();

			GLuint getTextureID();

		private:
			
			Glb::Shader* shader;
			
			float* data;

			GLuint VAO = 0;
			GLuint VBO = 0;
			GLuint EBO = 0;
			GLuint FBO = 0;
			GLuint RBO = 0;

			GLuint textureGridID = 0;
			GLuint texturePixelID = 0;

			GLuint smokeTexture = 0;
		};
	}
}

#endif // !__EULER_RENDERER_H__
