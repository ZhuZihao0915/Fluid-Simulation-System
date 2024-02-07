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
			Renderer(MACGrid3d& grid) :mGrid(grid) {};

			void init();
			void createTextureFromFramebuffer();

			void draw();
			void drawOneSheet();
			void drawXSheets();
			void drawYSheets();
			void drawZSheets();

			GLuint getImTextureIDByDensity();

			Glb::Camera mCamera;

		private:

			GLuint textureID = 0;

			bool backToFront;

			GLuint vaoXSheets;
			GLuint vaoYSheets;
			GLuint vaoZSheets;

			MACGrid3d& mGrid;

		};
	}
}

#endif // !__EULER_RENDERER_H__
