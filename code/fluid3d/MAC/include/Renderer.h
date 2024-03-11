#pragma once
#ifndef __MAC3D_RENDERER_H__
#define __MAC3D_RENDERER_H__


#include "glm/glm.hpp"
#include <glfw3.h>
#include "Shader.h"
#include "Container.h"
#include "MACGrid3d.h"
#include "Camera.h"
#include "Configure.h"

namespace FluidSimulation {
	namespace MAC3d {
		class Renderer {
		public:
			Renderer(MACGrid3d& grid);

			void loadTexture();

			void draw();
			void drawOneSheet();
			void drawOneSheetXY();
			void drawOneSheetYZ();
			void drawOneSheetXZ();
			void drawXYSheets();	// 平行于XY面的切面
			void drawYZSheets();
			void drawXZSheets();

			GLuint getTextureID();

		private:

			GLuint textureID = 0;
			GLuint smokeTexture = 0;

			float eps = 0.05f;

			GLuint VAO_XY;
			GLuint VBO_XY;

			GLuint VAO_YZ;
			GLuint VBO_YZ;

			GLuint VAO_XZ;
			GLuint VBO_XZ;

			GLuint VAO = 0;
			GLuint VBO = 0;
			GLuint EBO = 0;
			GLuint FBO = 0;
			GLuint RBO = 0;

			int width = imageWidth / 2;
			int height = imageHeight / 2;
			
			float* data;

			Glb::Shader* pixelShader;
			Glb::Shader* gridShader;
			Glb::Container* container;

			MACGrid3d& mGrid;

		};
	}
}

#endif // !__EULER_RENDERER_H__
