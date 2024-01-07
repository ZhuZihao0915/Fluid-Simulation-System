#pragma once
#ifndef __SPH_3D_RENDERER_H__
#define __SPH_3D_RENDERER_H__


#include <glad/glad.h>
#include "glfw3.h"

#include <vector>
#include "Shader.h"
#include <glm/glm.hpp>

#include "ComputeShader.h"
#include "ParticalSystem3d.h"

#include "Camera.h"
#include "SkyBox.h"

#include "DepthFilter.h"
#include "Material.h"
#include "ShadowMap.h"

namespace FluidSimulation {

    namespace SPH3d {
        class Renderer
        {
        public:
            Renderer();
            ~Renderer();

            int32_t Init();

            // �ϴ�����ȡ����
            void UploadUniforms(ParticalSystem3d& ps);
            void UploadParticalInfo(ParticalSystem3d& ps);
            void DumpParticalInfo(ParticalSystem3d& ps);

            // ��⡢��Ⱦ
            void SolveParticals();
            void Update();

            // window
            bool ShouldClose();
            void ProcessInput();
            void PollEvents();

        private:
            static void ResizeCallback(GLFWwindow* window, int width, int height);
            static void CursorPosCallBack(GLFWwindow* window, double xpos, double ypos);
            static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
            static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

        private:
            bool CreateWindow();
            void UpdateFPS();
            void BuildShaders();
            void InitFilters();
            void GenerateFrameBuffers();
            void GenerateBuffers();
            void GenerateTextures();
            void LoadSkyBox();
            void CreateRenderAssets();
            void MakeVertexArrays();
            void DrawParticals();
            int32_t Destroy();

        private:
            // window
            GLFWwindow* mWindow = nullptr;
            int mWindowWidth = 1000;
            int mWindowHeight = 1000;

            // camera
            Glb::Camera mCamera;
            bool mFirstMouseFlag = true;
            float mLastX;
            float mLastY;
            bool mLeftPressFlag = false;
            bool mRightPressFlag = false;
            bool mMiddlePressFlag = false;
            bool mPauseFlag = false;

            // shaders
            Glb::Shader* mScreenQuad = nullptr;
            Glb::Shader* mDrawColor3d = nullptr;
            Glb::ComputeShader* mComputeParticals = nullptr;
            Glb::Shader* mPointSpriteZValue = nullptr;
            Glb::Shader* mPointSpriteThickness = nullptr;
            Glb::Shader* mDrawFluidColor = nullptr;
            Glb::Shader* mDrawModel = nullptr;

            // fbo
            GLuint mFboDepth = 0;
            GLuint mTexZBuffer = 0;
            GLuint mRboDepthBuffer = 0;

            GLuint mFboThickness = 0;
            GLuint mTexThicknessBuffer = 0;

            // vao
            GLuint mVaoNull = 0;
            GLuint mVaoParticals = 0;
            GLuint mVaoCoord = 0;
            GLuint mVaoFloor = 0;

            // buffers
            GLuint mCoordVertBuffer = 0;
            GLuint mBufferParticals = 0;
            GLuint mBufferBlocks = 0;
            GLuint mBufferFloor = 0;

            // texures
            GLuint mTestTexture = 0;
            GLuint mTexKernelBuffer = 0;
            GLuint mTexZBlurTempBuffer = 0;

            // SkyBox
            Glb::SkyBox* mSkyBox = nullptr;

            // Materials
            Material* mSlabWhite = nullptr;

            // time statistics
            int32_t mParticalNum = 0;
            float_t mUpdateTime = 0.0f;
            float_t updateTitleTime = 0.0f;
            float_t frameCount = 0.0f;

            // shadow map
            Glb::PointLight mLight;
            Glb::ShadowMap* mShadowMap;
            Glb::DepthFilter* mDepthFilter;

            glm::vec3 mExternelAccleration = { 0.0, 0.0, 0.0 };
        };
    }
}

#endif // !RENDER_WIDGET_H
