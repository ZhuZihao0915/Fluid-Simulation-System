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
			Renderer(MACGrid3d& grid, Glb::Camera& c) :mGrid(grid), mCamera(c) {};

			void init();
			void createTextureFromFramebuffer();


			void draw();
			void drawOneSheet();
			void drawXYSheets();	// 平行于XY面的切面
			void drawYZSheets();
			void drawXZSheets();

			GLuint getImTextureIDByDensity();

			Glb::Camera& mCamera;

		private:

			GLuint textureID = 0;

			float eps = 0.05f;

			GLuint VAO_XY;
			GLuint VBO_XY;

			GLuint VAO_YZ;
			GLuint VBO_YZ;

			GLuint VAO_XZ;
			GLuint VBO_XZ;

			int width = 200;
			int height = 200;
			
			float* data;

			Glb::Shader* shader;

			GLuint fbo = 0;
			GLuint rbo = 0;

			MACGrid3d& mGrid;

		};
	}
}

#endif // !__EULER_RENDERER_H__
