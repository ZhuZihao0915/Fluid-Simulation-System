#include "MAC/include/Renderer.h"

namespace FluidSimulation {
	namespace MAC2d {

		float vertices[] = {
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f
		};

		Renderer::Renderer(){

			/*extern std::string shaderPath;
            std::string vertShaderPath = shaderPath + "/DrawSmoke2d.vert";
            std::string fragShaderPath = shaderPath + "/DrawSmoke2d.frag";
            shader = new Glb::Shader();
            shader->buildFromFile(vertShaderPath, fragShaderPath);

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glBindVertexArray(0);

            glGenFramebuffers(1, &FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, imageWidth, imageHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "ERROR: Framebuffer is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			data = new float[4 * width * height];*/



			glViewport(0, 0, imageWidth, imageHeight);

		}

		void Renderer::draw(MACGrid2d& mGrid) {

			/*glBindFramebuffer(GL_FRAMEBUFFER, FBO);

			glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			for (int j = height; j >= 1; j--) {
				for (int i = width; i >= 1; i--) {
					float pt_x = i * mGrid.mU.mMax[0] / (width);
					float pt_y = j * mGrid.mU.mMax[1] / (height);
					glm::vec2 pt(pt_x, pt_y);
					glm::vec4 color = mGrid.getRenderColor(pt);
					data[4 * ((height - j) * width + (width - i))] = color.r;
					data[4 * ((height - j) * width + (width - i)) + 1] = color.g;
					data[4 * ((height - j) * width + (width - i)) + 2] = color.b;
					data[4 * ((height - j) * width + (width - i)) + 3] = color.a;
				}
			}

			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
			glBindTexture(GL_TEXTURE_2D, 0);

			shader->use();
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(glGetUniformLocation(shader->getId(), "mTexture"), 0);

			shader->use();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);*/


			std::vector<float> imageData;

			for (int j = 1; j <= imageHeight; j++) {
				for (int i = 1; i <= imageWidth; i++) {
					float pt_x = i * mGrid.mU.mMax[0] / (imageWidth);
					float pt_y = j * mGrid.mU.mMax[1] / (imageHeight);
					glm::vec2 pt(pt_x, pt_y);
					glm::vec4 color = mGrid.getRenderColor(pt);
					imageData.push_back(color.x);
					imageData.push_back(color.y);
					imageData.push_back(color.z);
				}
			}

			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			// 设置纹理参数
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// 将颜色数据传递给纹理
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_FLOAT, imageData.data());
			glBindTexture(GL_TEXTURE_2D, 0);

		}

		GLuint Renderer::getTextureID() {
			
			return textureID;

		}
	}
}