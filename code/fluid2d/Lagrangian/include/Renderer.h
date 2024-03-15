#pragma once
#ifndef __LAGRANGIAN_2D_RENDERER_H__
#define __LAGRANGIAN_2D_RENDERER_H__

#include <glad/glad.h>
#include <glfw3.h>

#include <chrono>
#include <vector>
#include "Shader.h"
#include <glm/glm.hpp>

#include "ParticleSystem2d.h"
#include "Configure.h"

namespace FluidSimulation
{

    namespace Lagrangian2d
    {
        class Renderer
        {
        public:
            Renderer();

            int32_t init();

            void draw();

            int32_t Destroy();

            void PollEvents();

            void LoadVertexes(ParticleSystem2d &ps);

            GLuint GetRenderedTexture();

        private:
            float CalculateFPS();

            static void ResizeCallback(GLFWwindow *window, int width, int height);

        private:
            GLFWwindow *mWindow = nullptr;

            Glb::Shader *mParticleShader = nullptr;
            Glb::Shader *mSdfShader = nullptr;
            Glb::Shader *mMilkShader = nullptr;

            GLuint mVaoParticles = 0;
            GLuint mPositionBuffer = 0;
            GLuint mDensityBuffer = 0;

            GLuint fbo = 0;
            GLuint mTextureSdf = 0;
            GLuint mRboSdf = 0;

            size_t mParticleNum = 0;

            std::chrono::system_clock::time_point mUpdateTime;
        };
    }
}

#endif
