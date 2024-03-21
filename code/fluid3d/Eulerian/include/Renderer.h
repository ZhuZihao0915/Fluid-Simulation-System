#pragma once
#ifndef __EULERIAN_3D_RENDERER_H__
#define __EULERIAN_3D_RENDERER_H__


#include "glm/glm.hpp"
#include <glad/glad.h>
#include <glfw3.h>
#include "Shader.h"
#include "Container.h"
#include "MACGrid3d.h"
#include "Camera.h"
#include "Configure.h"

namespace FluidSimulation {
	namespace Eulerian3d {
		class Renderer {
		public:
			Renderer(MACGrid3d& grid);

			void loadTexture();
			void resetVertices(float x, float y, float z);
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

			int width = imageWidth;
			int height = imageHeight;
			
			float* data;

			Glb::Shader* pixelShader;
			Glb::Shader* gridShader;
			Glb::Container* container;

			MACGrid3d& mGrid;

		};
	}
}

#endif // !__EULER_RENDERER_H__
