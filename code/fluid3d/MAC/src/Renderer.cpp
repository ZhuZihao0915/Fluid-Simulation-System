#include "Renderer.h"
#include "fluid3d/MAC/include/Renderer.h"

namespace FluidSimulation {
	namespace MAC3d {

		float vertices[] = {
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};

		void Renderer::init()
		{

			std::string particalVertShaderPath = shaderPath + "/DrawSmoke3d.vert";
			std::string particalFragShaderPath = shaderPath + "/DrawSmoke3d.frag";
			shader = new Glb::Shader();
			shader->buildFromFile(particalVertShaderPath, particalFragShaderPath);


			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			createTextureFromFramebuffer();

			data = new float[4 * width * height];
		}

		void Renderer::createTextureFromFramebuffer()
		{
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
		}

		void Renderer::draw() {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			
			drawXSheets();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//drawOneSheet();
		}


		void Renderer::drawOneSheet()
		{
			std::vector<float> imageData;

			for (int j = 1; j <= imageHeight; j++) {
				for (int i = 1; i <= imageWidth; i++) {
					float pt_x = i * mGrid.mU.mMax[0] / (imageWidth);
					float pt_y = j * mGrid.mU.mMax[1] / (imageHeight);
					float pt_z = mGrid.mU.mMax[1] / 2;
					glm::vec3 pt(pt_x, pt_y, pt_z);
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

		void Renderer::drawXSheets()
		{

			for (float k = 0; k <= mGrid.mU.mMax[2];k+=mGrid.mU.mMax[2]/(numSheets-1)) {
				
				for (int j = 1; j <= height; j++) {
					for (int i = 1; i <= width; i++) {
						float pt_x = i * mGrid.mU.mMax[0] / (width);
						float pt_y = j * mGrid.mU.mMax[1] / (height);
						float pt_z = k;
						glm::vec3 pt(pt_x, pt_y, pt_z);
						glm::vec4 color = mGrid.getRenderColor(pt);
						data[4 * ((j - 1) * width + (i - 1))] = color.r;
						data[4 * ((j - 1) * width + (i - 1))+1] = color.g;
						data[4 * ((j - 1) * width + (i - 1))+2] = color.b;
						data[4 * ((j - 1) * width + (i - 1))+3] = color.a;
					}
				}

				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
				// set the texture wrapping parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				// set texture filtering parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				shader->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glUniform1i(glGetUniformLocation(shader->getId(), "aTexture"), 0);

				glm::mat4 view = mCamera.GetView();
				glm::mat4 projection = glm::mat4(1.0f);
				projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, k));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO);
				shader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);

			}

		}

		GLuint Renderer::getImTextureIDByDensity() {
			return textureID;
		}

	}
}