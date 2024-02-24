#pragma once
#ifndef __SPH_2D_RENDERER_H__
#define __SPH_2D_RENDERER_H__


#include <glad/glad.h>
#include <glfw3.h>

#include <chrono>
#include <vector>
#include "Shader.h"
#include <glm/glm.hpp>

#include "ParticalSystem2d.h"
#include "Configure.h"

namespace FluidSimulation {

    namespace SPH2d {
        class Renderer
        {
        public:
            Renderer();

            void draw(ParticalSystem2d& ps);

            void destroy();

            GLuint GetRenderedTexture();

        private:

            Glb::Shader* shader = nullptr;

            GLuint VAO = 0;
            GLuint positionBuffer = 0;
            GLuint densityBuffer = 0;

            GLuint FBO = 0;
            GLuint RBO = 0;

            GLuint textureID = 0;

            size_t particalNum = 0;

        };
    }
}

#endif

