#include "Lagrangian/include/Renderer.h"

#include <iostream>
#include <fstream>
#include "Configure.h"

// This is the main definitions of renderer, which involve the OpenGL functions.
// Before browsing this file, make sure you have elementary knowledge of OpenGL.

namespace FluidSimulation
{

    namespace Lagrangian2d
    {

        Renderer::Renderer()
        {
        }

        int32_t Renderer::init()
        {
            extern std::string shaderPath;

            std::string particleVertShaderPath = shaderPath + "/DrawParticles2d.vert";
            std::string particleFragShaderPath = shaderPath + "/DrawParticles2d.frag";
            mParticleShader = new Glb::Shader();
            mParticleShader->buildFromFile(particleVertShaderPath, particleFragShaderPath);

            // generate vertex array object
            glGenVertexArrays(1, &mVaoParticles);
            // generate vertex buffer object (for position)
            glGenBuffers(1, &mPositionBuffer);
            // generate vertex buffer object (for density)
            glGenBuffers(1, &mDensityBuffer);

            // generate frame buffer object
            glGenFramebuffers(1, &fbo);
            // make it active
            // start fbo
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            // generate textures
            glGenTextures(1, &mTextureSdf);
            glBindTexture(GL_TEXTURE_2D, mTextureSdf);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glBindTexture(GL_TEXTURE_2D, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureSdf, 0);

            // generate render buffer object (RBO)
            glGenRenderbuffers(1, &mRboSdf);
            glBindRenderbuffer(GL_RENDERBUFFER, mRboSdf);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, imageWidth, imageHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRboSdf);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cout << "ERROR: SDF Framebuffer is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // end fbo

            glViewport(0, 0, imageWidth, imageHeight);

            return 0;
        }

        void Renderer::draw()
        {

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(mVaoParticles); // VAO
            mParticleShader->use();           // shader
            glEnable(GL_PROGRAM_POINT_SIZE);

            glDrawArrays(GL_POINTS, 0, mParticleNum);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        int32_t Renderer::Destroy()
        {

            glDeleteVertexArrays(1, &mVaoParticles);
            glDeleteBuffers(1, &mPositionBuffer);
            glDeleteBuffers(1, &mDensityBuffer);
            delete mParticleShader;
            delete mSdfShader;
            delete mMilkShader;
            glfwTerminate();
            return 0;
        }

        void Renderer::PollEvents()
        {
            glfwPollEvents();
        }

        void Renderer::LoadVertexes(ParticleSystem2d &ps)
        {
            // bind VAO (decide which VAO we want to set)
            glBindVertexArray(mVaoParticles);

            // bind VBO to GL_ARRAY_BUFFER
            glBindBuffer(GL_ARRAY_BUFFER, mPositionBuffer);
            // copy data to the current bound buffer(VBO)
            glBufferData(GL_ARRAY_BUFFER, ps.mParticleInfos.size() * sizeof(ParticleInfo2d), ps.mParticleInfos.data(), GL_STATIC_DRAW);

            // tell OpenGL how to parse vertex data. here are the parameters' meaning:
            // 0 is the VBO's location of current VAO, which is defined in shader
            // 2 means vec2
            // GL_FLOAT implies that vec is composed of float
            // GL_FALSE means we don't want normalization
            // stride, the stride of two vertex data
            // offset
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleInfo2d), (void *)offsetof(ParticleInfo2d, position));
            // activate
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleInfo2d), (void *)offsetof(ParticleInfo2d, density));
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);

            mParticleNum = ps.mParticleInfos.size();
        }

        GLuint Renderer::GetRenderedTexture()
        {
            return mTextureSdf;
        }
    }

}