#include "fluid3d/Lagrangian/include/Renderer.h"

namespace FluidSimulation
{
    namespace Lagrangian3d
    {

        const glm::vec3 vertexes[] = {
            glm::vec3(0.0, 0.0, 0.0),
            glm::vec3(1.0, 0.0, 0.0),
            glm::vec3(0.0, 1.0, 0.0),
            glm::vec3(0.0, 0.0, 1.0)};

        const GLuint indices[] = {
            0, 1, 0,
            2, 0, 3
        };
        

        void Renderer::init()
        {

            container = new Glb::Container();
            container->init();

            // Build Shaders

            mDrawColor3d = new Glb::Shader();
            std::string drawColorVertPath = shaderPath + "/DrawParticles3d.vert";
            std::string drawColorFragPath = shaderPath + "/DrawParticles3d.frag";
            mDrawColor3d->buildFromFile(drawColorVertPath, drawColorFragPath);

            // Generate Frame Buffers
            // generate frame buffer object
            glGenFramebuffers(1, &FBO);
            // make it active
            // start fbo
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            // generate textures
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glBindTexture(GL_TEXTURE_2D, 0);

      
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

            // generate render buffer object (RBO)
            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // RBO�󶨵�FBO
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cout << "ERROR: SDF Framebuffer is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Generate Buffer
            glGenBuffers(1, &mBufferParticles);

            // GenerateTextures();

            LoadSkyBox();

            MakeVertexArrays();

            glGenVertexArrays(1, &mVaoNull);
            glEnable(GL_MULTISAMPLE);

            glViewport(0, 0, imageWidth, imageHeight);
        }

        void Renderer::GenerateFrameBuffers()
        {
            // NEW!!!
            // generate frame buffer object
            glGenFramebuffers(1, &FBO);
            // make it active
            // start fbo
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            // generate textures
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Texture2D
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

            // generate render buffer object (RBO)
            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);


            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cout << "ERROR: SDF Framebuffer is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // END!!!
        }

        void Renderer::GenerateBuffers()
        {
            glGenBuffers(1, &mBufferParticles); // ssbo
        }

        void Renderer::GenerateTextures()
        {
            glGenTextures(1, &mTestTexture);
            glBindTexture(GL_TEXTURE_2D, mTestTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 100, 100, 0, GL_RGBA, GL_FLOAT, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void Renderer::LoadSkyBox()
        {
            mSkyBox = new Glb::SkyBox();
            mSkyBox->Create();
            std::vector<std::string> paths{
                picturePath + "/right.jpg",
                picturePath + "/left.jpg",
                picturePath + "/top.jpg",
                picturePath + "/bottom.jpg",
                picturePath + "/front.jpg",
                picturePath + "/back.jpg"};
            mSkyBox->LoadImages(paths);
            mSkyBox->BuildShader();
        }


        void Renderer::MakeVertexArrays()
        {
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, mBufferParticles);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleInfo3d), (void *)offsetof(ParticleInfo3d, position));
            glEnableVertexAttribArray(0); // location = 0
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleInfo3d), (void *)offsetof(ParticleInfo3d, density));
            glEnableVertexAttribArray(1); // location = 1
            glBindVertexArray(0);
        }

        void Renderer::load(ParticleSystem3d &ps)
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferParticles);
            glBufferData(GL_SHADER_STORAGE_BUFFER, ps.mParticleInfos.size() * sizeof(ParticleInfo3d), ps.mParticleInfos.data(), GL_DYNAMIC_COPY);
            particleNum = ps.mParticleInfos.size();
        }

        void Renderer::draw()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_PROGRAM_POINT_SIZE);

            mDrawColor3d->use();
            mDrawColor3d->setMat4("view", Glb::Camera::getInstance().GetView());
            mDrawColor3d->setMat4("projection", Glb::Camera::getInstance().GetProjection());

            glBindVertexArray(mVaoCoord);
            glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, indices);
            glBindVertexArray(VAO);
            // glDrawArraysInstanced(GL_TRIANGLES, 0, 36, particleNum);
            glDrawArrays(GL_POINTS, 0, particleNum);
            // mSkyBox->Draw(mWindow, mVaoNull, Glb::Camera::getInstance().GetView(), Glb::Camera::getInstance().GetProjection());
            mDrawColor3d->unUse();

            container->draw();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint Renderer::getRenderedTexture()
        {
            return textureID;
        }
    }
}