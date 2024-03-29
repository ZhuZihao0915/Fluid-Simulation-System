#pragma once
#ifndef __LAGRANGIAN_3D_RENDERER_H__
#define __LAGRANGIAN_3D_RENDERER_H__

#include <glm/glm.hpp>

#include <glad/glad.h>
#include "glfw3.h"
#include "Shader.h"
#include "Container.h"

#include "Camera.h"
#include "Global.h"

#include "Configure.h"

#include "ParticleSystem3d.h"

namespace FluidSimulation
{

    namespace Lagrangian3d
    {

        class Renderer
        {
        public:
            Renderer(){};

            void init();

            GLuint getRenderedTexture();

            void load(ParticleSystem3d &ps);
            void draw();

        private:
            void GenerateFrameBuffers();
            void GenerateBuffers();
            void GenerateTextures();
            void MakeVertexArrays();
            int32_t Destroy();

        private:
            // window
            GLFWwindow *mWindow = nullptr;
            int mWindowWidth = 1000;
            int mWindowHeight = 1000;

            // camera
            bool mFirstMouseFlag = true;
            float mLastX;
            float mLastY;
            bool mLeftPressFlag = false;
            bool mRightPressFlag = false;
            bool mMiddlePressFlag = false;
            bool mPauseFlag = false;

            // shaders
            Glb::Shader *mDrawColor3d = nullptr;

            Glb::Container *container = nullptr;

            GLuint FBO = 0;
            GLuint RBO = 0;

            // fbo
            GLuint mFboDepth = 0;
            GLuint mTexZBuffer = 0;
            GLuint mRboDepthBuffer = 0;
            GLuint mFboThickness = 0;
            GLuint mTexThicknessBuffer = 0;

            // vao
            GLuint mVaoNull = 0;
            GLuint VAO = 0;
            GLuint mVaoCoord = 0;
            GLuint mVaoFloor = 0;

            // buffers
            GLuint mCoordVertBuffer = 0;
            GLuint mBufferParticles = 0;
            GLuint mBufferBlocks = 0;
            GLuint mBufferFloor = 0;

            // texures
            GLuint textureID = 0;
            GLuint mTestTexture = 0;
            GLuint mTexKernelBuffer = 0;
            GLuint mTexZBlurTempBuffer = 0;

            // time statistics
            int32_t particleNum = 0;
            float_t mUpdateTime = 0.0f;
            float_t updateTitleTime = 0.0f;
            float_t frameCount = 0.0f;

            glm::vec3 mExternelAccleration = {0.0, 0.0, 0.0};
        };
    }
}

#endif
