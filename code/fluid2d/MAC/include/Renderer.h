#pragma once
#ifndef __MAC2D_RENDERER_H__
#define __MAC2D_RENDERER_H__

#include "MAC/include/MACGrid2d.h"
#include <glfw3.h>
#include "Configure.h"
#include "glm/glm.hpp"

namespace FluidSimulation {
	namespace MAC2d {
		class Renderer {
		public:
			Renderer(MACGrid2d& grid): mGrid(grid){

			}

			GLuint getImTextureIDByDensity();
			void Renderer::applyFilter(std::vector<float>& imageData, int width, int height);
		private:
			MACGrid2d& mGrid;
		
		};
	}
}

#endif // !__EULER_RENDERER_H__
