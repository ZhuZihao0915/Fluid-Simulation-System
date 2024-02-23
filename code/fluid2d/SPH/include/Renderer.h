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

            int32_t Init();

            void draw();

            int32_t Destroy();

            void PollEvents();

            void LoadVertexes(ParticalSystem2d& ps);

            GLuint GetRenderedTexture();

        private:

            GLFWwindow* mWindow = nullptr;

            Glb::Shader* mParticalShader = nullptr;
            Glb::Shader* mSdfShader = nullptr;
            Glb::Shader* mMilkShader = nullptr;

            GLuint mVaoParticals = 0;
            GLuint mPositionBuffer = 0;
            GLuint mDensityBuffer = 0;

            GLuint fbo = 0;
            GLuint mTextureSdf = 0;
            GLuint mRboSdf = 0;

            size_t mParticalNum = 0;

            std::chrono::system_clock::time_point mUpdateTime;


        };
    }
}

#endif

