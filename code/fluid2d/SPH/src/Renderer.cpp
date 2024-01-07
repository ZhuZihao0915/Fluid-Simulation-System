#include "SPH/include/Renderer.h"

#include <iostream>
#include <fstream>
#include "Configure.h"

// This is the main definitions of renderer, which involve the OpenGL functions.
// Before browsing this file, make sure you have elementary knowledge of OpenGL.

namespace FluidSimulation {

    namespace SPH2d {

        Renderer::Renderer() {

        }

        int32_t Renderer::Init() {

            //glfwSetWindowUserPointer(window, this);
            //glfwSetFramebufferSizeCallback(window, ResizeCallback);

            // 初始化shader

            extern std::string shaderPath;

            std::string particalVertShaderPath = shaderPath + "/DrawParticals.vert";
            std::string particalFragShaderPath = shaderPath + "/DrawParticals.frag";
            mParticalShader = new Glb::Shader();
            mParticalShader->BuildFromFile(particalVertShaderPath, particalFragShaderPath);

            std::string ballVertShaderPath = shaderPath + "/DrawSdf.vert";
            std::string ballGeomShaderPath = shaderPath + "/DrawSdf.geom";
            std::string ballFragShaderPath = shaderPath + "/DrawSdf.frag";
            mSdfShader = new Glb::Shader();
            mSdfShader->BuildFromFile(ballVertShaderPath, ballFragShaderPath, ballGeomShaderPath);

            std::string milkVertShaderPath = shaderPath + "/DrawMilk.vert";
            std::string milkFragShaderPath = shaderPath + "/DrawMilk.frag";
            mMilkShader = new Glb::Shader();
            mMilkShader->BuildFromFile(milkVertShaderPath, milkFragShaderPath);

            glUseProgram(mMilkShader->GetId());
            glUniform1i(glGetUniformLocation(mMilkShader->GetId(), "textureSdf"), 0);

            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << error << std::endl;
            }

            // generate vertex array object
            glGenVertexArrays(1, &mVaoParticals);
            // generate vertex buffer object (for position)
            glGenBuffers(1, &mPositionBuffer);
            // generate vertex buffer object (for density)
            glGenBuffers(1, &mDensityBuffer);

            // generate frame buffer object
            glGenFramebuffers(1, &mFboSdf);
            // make it active
            glBindFramebuffer(GL_FRAMEBUFFER, mFboSdf);


            error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << error << std::endl;
            }

            // generate textures
            glGenTextures(1, &mTextureSdf);
            glBindTexture(GL_TEXTURE_2D, mTextureSdf);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glBindTexture(GL_TEXTURE_2D, 0);

            // 绑定到FBO
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureSdf, 0);

            // generate render buffer object (RBO)
            glGenRenderbuffers(1, &mRboSdf);
            glBindRenderbuffer(GL_RENDERBUFFER, mRboSdf);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // 绑定到FBO
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRboSdf);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "ERROR: SDF Framebuffer is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // 视口大小
            glViewport(0, 0, mWindowWidth, mWindowHeight);

            error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << error << std::endl;
            }


            return 0;
        }

        void Renderer::Update() {
            // 画粒子
            /*glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(mVaoParticals);
            mParticalShader->Use();
            glEnable(GL_PROGRAM_POINT_SIZE);
            glDrawArrays(GL_POINTS, 0, mParticalNum);*/

            // 画粒子球
            glBindFramebuffer(GL_FRAMEBUFFER, mFboSdf);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(mVaoParticals);
            mParticalShader->Use();
            //mSdfShader->Use();
            glEnable(GL_PROGRAM_POINT_SIZE);
            glDrawArrays(GL_POINTS, 0, mParticalNum);

            // 画牛奶
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            /*glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            mMilkShader->Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mTextureSdf);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);*/

            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << error << std::endl;
            }
        }

        int32_t Renderer::Destroy() {

            glDeleteVertexArrays(1, &mVaoParticals);
            glDeleteBuffers(1, &mPositionBuffer);
            glDeleteBuffers(1, &mDensityBuffer);
            delete mParticalShader;
            delete mSdfShader;
            delete mMilkShader;
            glfwTerminate();
            return 0;
        }

        void Renderer::PollEvents() {
            glfwPollEvents();
        }

        void Renderer::LoadVertexes(ParticalSystem2d& ps) {
            // bind VAO (decide which VAO we want to set)
            glBindVertexArray(mVaoParticals);
            // bind VBO to GL_ARRAY_BUFFER
            glBindBuffer(GL_ARRAY_BUFFER, mPositionBuffer);
            // copy data to the current bound buffer(VBO)
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * ps.positions.size(), ps.positions.data(), GL_STATIC_DRAW);

            // tell OpenGL how to parse vertex data. here are the parameters' meaning:
            // 0 is the VBO's location of current VAO, which is defined in shader
            // 2 means vec2
            // GL_FLOAT implies that vec is composed of float
            // GL_FALSE means we don't want normalization
            // stride, the stride of two vertex data
            // offset
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

            // activate
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, mDensityBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ps.mDensity.size(), ps.mDensity.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);

            mParticalNum = ps.positions.size();
        }


        float Renderer::CalculateFPS() {
            auto nowTime = std::chrono::system_clock::now();
            auto deltaTime = nowTime - mUpdateTime;
            mUpdateTime = nowTime;
            auto durMS = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count();
            float fps = 1000.0f / durMS;
            return fps;
        }

        void Renderer::ResizeCallback(GLFWwindow* window, int width, int height) {
            auto thisPtr = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
            glViewport(0, 0, width, height);
        }

        GLuint Renderer::GetRenderedTexture() {
            return mTextureSdf;
        }
    }

}