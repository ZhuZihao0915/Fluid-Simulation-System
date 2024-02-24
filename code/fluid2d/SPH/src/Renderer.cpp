#include "SPH/include/Renderer.h"

#include <iostream>
#include <fstream>
#include "Configure.h"

// This is the main definitions of renderer, which involve the OpenGL functions.
// Before browsing this file, make sure you have elementary knowledge of OpenGL.

namespace FluidSimulation {

    namespace SPH2d {

        Renderer::Renderer() {
            std::string particalVertShaderPath = shaderPath + "/DrawParticals.vert";
            std::string particalFragShaderPath = shaderPath + "/DrawParticals.frag";
            shader = new Glb::Shader();
            shader->buildFromFile(particalVertShaderPath, particalFragShaderPath);

            // generate vertex array object
            glGenVertexArrays(1, &VAO);
            // generate vertex buffer object (for position)
            glGenBuffers(1, &positionBuffer);
            // generate vertex buffer object (for density)
            glGenBuffers(1, &densityBuffer);

            // generate frame buffer object
            glGenFramebuffers(1, &FBO);
            // make it active
            // start fbo
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            // generate textures
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Texture2D�󶨵�FBO
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

            // generate render buffer object (RBO)
            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, imageWidth, imageHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // RBO�󶨵�FBO
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "ERROR: Framebuffer is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // end fbo

            glViewport(0, 0, imageWidth, imageHeight);
        }

        
        void Renderer::draw(ParticalSystem2d& ps) {

            // bind VAO (decide which VAO we want to set)
            glBindVertexArray(VAO);

            // bind VBO to GL_ARRAY_BUFFER
            glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
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
            glEnableVertexAttribArray(0);   // ��Ӧshader�е�lacation

            // ��VBO��GL_ARRAY_BUFFER��������GL_ARRAY_BUFFER�Ĳ������ǶԸ�VBO�Ĳ���
            glBindBuffer(GL_ARRAY_BUFFER, densityBuffer);
            // �����ݸ��Ƶ�������ڴ���
            /* ���ĸ�����ָ��������ϣ���Կ���ι�����������ݡ�����������ʽ��

                GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
                GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
                GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣
            */
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ps.mDensity.size(), ps.mDensity.data(), GL_STATIC_DRAW);
            // ����OpenGL����ν�����������
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);

            particalNum = ps.positions.size();


            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(VAO);
            shader->use();
            glEnable(GL_PROGRAM_POINT_SIZE);

            glDrawArrays(GL_POINTS, 0, particalNum);   

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Renderer::destroy() {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &positionBuffer);
            glDeleteBuffers(1, &densityBuffer);
            glDeleteFramebuffers(1, &FBO);
            glDeleteRenderbuffers(1, &RBO);
            delete shader;
        }

        GLuint Renderer::GetRenderedTexture() {
            return textureID;
        }
    }

}