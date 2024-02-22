#include "fluid3d/SPH/include/Renderer.h"

namespace FluidSimulation {
	namespace SPH3d {

        void Renderer::Init() {

            BuildShaders();
            GenerateFrameBuffers();
            GenerateBuffers();
            GenerateTextures();
            LoadSkyBox();
            MakeVertexArrays(); // 生成画粒子的vao

            glGenVertexArrays(1, &mVaoNull);
            glEnable(GL_MULTISAMPLE);
		}

        void Renderer::BuildShaders() {
            mDrawColor3d = new Glb::Shader();
            std::string drawColorVertPath = shaderPath + "/DrawColor3d.vert";
            std::string drawColorFragPath = shaderPath + "/DrawColor3d.frag";
            //std::string drawColorGeomPath = shaderPath + "/DrawColor3d.geom";
            mDrawColor3d->buildFromFile(drawColorVertPath, drawColorFragPath);// , drawColorGeomPath);
        }

        void Renderer::GenerateFrameBuffers() {
            // NEW!!!
            // generate frame buffer object
            glGenFramebuffers(1, &fbo);
            // make it active
            // start fbo
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            // generate textures
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Texture2D绑定到FBO
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

            // generate render buffer object (RBO)
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // RBO绑定到FBO
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "ERROR: SDF Framebuffer is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // END!!!

            // depth framebuffer
            glGenFramebuffers(1, &mFboDepth);

            glGenTextures(1, &mTexZBuffer);
            glBindTexture(GL_TEXTURE_2D, mTexZBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWindowWidth, mWindowHeight, 0, GL_RED, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);

            glGenRenderbuffers(1, &mRboDepthBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, mRboDepthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, mFboDepth);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexZBuffer, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRboDepthBuffer);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "ERROR: mFboDepth is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // thickness framebuffer
            glGenFramebuffers(1, &mFboThickness);

            glGenTextures(1, &mTexThicknessBuffer);
            glBindTexture(GL_TEXTURE_2D, mTexThicknessBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWindowWidth, mWindowHeight, 0, GL_RED, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, mFboThickness);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexThicknessBuffer, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRboDepthBuffer);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "ERROR: mFboThickness is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Renderer::GenerateBuffers() {
            glGenBuffers(1, &mBufferParticals);     // ssbo
        }

        void Renderer::GenerateTextures() {
            // 测试用的纹理
            glGenTextures(1, &mTestTexture);
            glBindTexture(GL_TEXTURE_2D, mTestTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 100, 100, 0, GL_RGBA, GL_FLOAT, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);

            // 核函数纹理
            glGenTextures(1, &mTexKernelBuffer);
            glBindTexture(GL_TEXTURE_1D, mTexKernelBuffer);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_1D, 0);

            // 模糊Z后的坐标图
            glGenTextures(1, &mTexZBlurTempBuffer);
            glBindTexture(GL_TEXTURE_2D, mTexZBlurTempBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWindowWidth, mWindowHeight, 0, GL_RED, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);

        }

        void Renderer::LoadSkyBox() {
            mSkyBox = new Glb::SkyBox();
            mSkyBox->Create();
            std::vector<std::string> paths
            {
                "../../../../code/resources/skybox/right.jpg",
                "../../../../code/resources/skybox/left.jpg",
                "../../../../code/resources/skybox/top.jpg",
                "../../../../code/resources/skybox/bottom.jpg",
                "../../../../code/resources/skybox/front.jpg",
                "../../../../code/resources/skybox/back.jpg"
            };
            mSkyBox->LoadImages(paths);
            mSkyBox->BuildShader();
        }

        // 生成需要渲染的object的VAO
        void Renderer::MakeVertexArrays() {
            // 粒子
            glGenVertexArrays(1, &mVaoParticals);
            glBindVertexArray(mVaoParticals);
            glBindBuffer(GL_ARRAY_BUFFER, mBufferParticals);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticalInfo3d), (void*)offsetof(ParticalInfo3d, position));
            glEnableVertexAttribArray(0);   // location = 0
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ParticalInfo3d), (void*)offsetof(ParticalInfo3d, density));
            glEnableVertexAttribArray(1);   // location = 1
            glBindVertexArray(0);
        }

        void Renderer::load(ParticalSystem3d& ps) {
            // 申请装粒子信息的buffer
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferParticals);
            glBufferData(GL_SHADER_STORAGE_BUFFER, ps.mParticalInfos.size() * sizeof(ParticalInfo3d), ps.mParticalInfos.data(), GL_DYNAMIC_COPY);
            mParticalNum = ps.mParticalInfos.size();
        }

        void Renderer::draw() {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_PROGRAM_POINT_SIZE);

            mDrawColor3d->use();
            mDrawColor3d->setMat4("view", mCamera.GetView());
            mDrawColor3d->setMat4("projection", mCamera.GetProjection());

            glBindVertexArray(mVaoCoord);
            glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, SPH3dPara::indices);
            glBindVertexArray(mVaoParticals);
            //glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mParticalNum);
            glDrawArrays(GL_POINTS, 0, mParticalNum);
            mSkyBox->Draw(mWindow, mVaoNull, mCamera.GetView(), mCamera.GetProjection());
            mDrawColor3d->unUse();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint Renderer::getRenderedTexture() {
            return textureID;
        }
	}
}