#pragma once
#ifndef __MAC3D_RENDERER_H__
#define __MAC3D_RENDERER_H__

#include "glm/glm.hpp"
#include <glfw3.h>

#include "Shader.h"
#include "MACGrid3d.h"
#include "Camera.h"
#include "Configure.h"

namespace FluidSimulation {
	namespace MAC3d {
		class Renderer {
		public:
			Renderer(MACGrid3d& grid);

			void draw();
			void drawOneSheet();
			void drawXYSheets();	// 平行于XY面的切面
			void drawYZSheets();
			void drawXZSheets();

			GLuint getTextureID();

		private:

			GLuint textureID = 0;

			float eps = 0.05f;

			GLuint VAO_XY;
			GLuint VBO_XY;

			GLuint VAO_YZ;
			GLuint VBO_YZ;

			GLuint VAO_XZ;
			GLuint VBO_XZ;

			int width = 600;
			int height = 600;
			
			float* data;

			Glb::Shader* shader;

			GLuint FBO = 0;
			GLuint RBO = 0;

			MACGrid3d& mGrid;

		};
	}
}

#endif // !__EULER_RENDERER_H__
