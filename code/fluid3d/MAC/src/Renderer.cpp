#include "Renderer.h"
#include "Renderer.h"
#include "Renderer.h"
#include "Renderer.h"
#include "fluid3d/MAC/include/Renderer.h"

namespace FluidSimulation {
	namespace MAC3d {

		float verticesXY[] = {
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};

		float verticesYZ[] = {
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};

		float verticesXZ[] = {
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f
		};

		Renderer::Renderer(MACGrid3d& grid) :mGrid(grid)
		{

			std::string particalVertShaderPath = shaderPath + "/DrawSmoke3d.vert";
			std::string particalFragShaderPath = shaderPath + "/DrawSmoke3d.frag";
			shader = new Glb::Shader();
			shader->buildFromFile(particalVertShaderPath, particalFragShaderPath);


			glGenVertexArrays(1, &VAO_XY);
			glGenBuffers(1, &VBO_XY);

			glBindVertexArray(VAO_XY);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_XY);
			glBufferData(GL_ARRAY_BUFFER, sizeof(verticesXY), verticesXY, GL_STATIC_DRAW);

			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glBindVertexArray(0);


			glGenVertexArrays(1, &VAO_YZ);
			glGenBuffers(1, &VBO_YZ);
			glBindVertexArray(VAO_YZ);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_YZ);
			glBufferData(GL_ARRAY_BUFFER, sizeof(verticesYZ), verticesYZ, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glBindVertexArray(0);

			glGenVertexArrays(1, &VAO_XZ);
			glGenBuffers(1, &VBO_XZ);
			glBindVertexArray(VAO_XZ);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_XZ);
			glBufferData(GL_ARRAY_BUFFER, sizeof(verticesXZ), verticesXZ, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glBindVertexArray(0);


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

			// Texture2D绑定到FBO
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

			// generate render buffer object (RBO)
			glGenRenderbuffers(1, &RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, imageWidth, imageHeight);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			// RBO绑定到FBO
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "ERROR: Framebuffer is not complete!" << std::endl;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);


			// 如果开启深度测试，就只能绘制一个方向的平面
			glDisable(GL_DEPTH_TEST);

			glEnable(GL_BLEND); //开启混合模式
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //设置混合函数

			data = new float[4 * width * height];

			glViewport(0, 0, imageWidth, imageHeight);
		}
		

		void Renderer::draw() {
			glBindFramebuffer(GL_FRAMEBUFFER, FBO);
			
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (MAC3dPara::oneSheet) {
				drawOneSheet();
			}
			else {
				if (MAC3dPara::xySheetsON) {
					drawXYSheets();
				}
				if (MAC3dPara::yzSheetsON)
				{
					drawYZSheets();
				}
				if (MAC3dPara::xzSheetsON) {
					drawXZSheets();
				}
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
		}

		void Renderer::drawOneSheet()
		{
			if (MAC3dPara::xySheetsON) {

				Glb::Timer::getInstance().start();

				for (int j = 1; j <= height; j++) {
					for (int i = 1; i <= width; i++) {
						float pt_x = i * mGrid.mU.mMax[0] / (width);
						float pt_y = j * mGrid.mV.mMax[1] / (height);
						float pt_z = MAC3dPara::distance * mGrid.mW.mMax[2];
						glm::vec3 pt(pt_x, pt_y, pt_z);
						glm::vec4 color = mGrid.getRenderColor(pt);
						data[4 * ((j - 1) * width + (i - 1))] = color.r;
						data[4 * ((j - 1) * width + (i - 1)) + 1] = color.g;
						data[4 * ((j - 1) * width + (i - 1)) + 2] = color.b;
						data[4 * ((j - 1) * width + (i - 1)) + 3] = color.a;
					}
				}

				Glb::Timer::getInstance().recordTime("get texture");

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

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, MAC3dPara::distance));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_XY);
				shader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);

				Glb::Timer::getInstance().recordTime("rendering");

			}
			if (MAC3dPara::yzSheetsON)
			{
				for (int k = height; k >= 1; k--) {
					for (int j = 1; j <= width; j++) {
						float pt_x = MAC3dPara::distance * mGrid.mU.mMax[0];
						float pt_y = j * mGrid.mV.mMax[1] / (width);
						float pt_z = k * mGrid.mW.mMax[2] / (height);
						glm::vec3 pt(pt_x, pt_y, pt_z);
						glm::vec4 color = mGrid.getRenderColor(pt);
						data[4 * ((height - k) * width + (j - 1))] = color.r;
						data[4 * ((height - k) * width + (j - 1)) + 1] = color.g;
						data[4 * ((height - k) * width + (j - 1)) + 2] = color.b;
						data[4 * ((height - k) * width + (j - 1)) + 3] = color.a;
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

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(MAC3dPara::distance, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_YZ);
				shader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);

			}
			if (MAC3dPara::xzSheetsON) {
				for (int k = height; k >= 1; k--) {
					for (int i = width; i >= 1; i--) {
						float pt_x = i * mGrid.mU.mMax[0] / (width);
						float pt_y = MAC3dPara::distance * mGrid.mV.mMax[1];
						float pt_z = k * mGrid.mW.mMax[2] / (height);
						glm::vec3 pt(pt_x, pt_y, pt_z);
						glm::vec4 color = mGrid.getRenderColor(pt);
						data[4 * ((height - k) * width + (width - i))] = color.r;
						data[4 * ((height - k) * width + (width - i)) + 1] = color.g;
						data[4 * ((height - k) * width + (width - i)) + 2] = color.b;
						data[4 * ((height - k) * width + (width - i)) + 3] = color.a;
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

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, MAC3dPara::distance, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_XZ);
				shader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);

			}
		}

		void Renderer::drawXYSheets()
		{

			for (float k = eps; k <= mGrid.mW.mMax[2];k+=(mGrid.mW.mMax[2]-2*eps)/(MAC3dPara::xySheetsNum-1)) {
				
				for (int j = 1; j <= height; j++) {
					for (int i = 1; i <= width; i++) {
						float pt_x = i * mGrid.mU.mMax[0] / (width);
						float pt_y = j * mGrid.mV.mMax[1] / (height);
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

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, k/ mGrid.mW.mMax[2]));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_XY);
				shader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);

			}

		}

		void Renderer::drawYZSheets()
		{

			for (float i = eps; i <= mGrid.mU.mMax[0]; i += (mGrid.mU.mMax[0]-2*eps) / (MAC3dPara::yzSheetsNum - 1)) {
				for (int k = height; k >= 1; k--) {
					for (int j = 1; j <= width; j++) {
						float pt_x = i;
						float pt_y = j * mGrid.mV.mMax[1] / (width);
						float pt_z = k * mGrid.mW.mMax[2] / (height);
						glm::vec3 pt(pt_x, pt_y, pt_z);
						glm::vec4 color = mGrid.getRenderColor(pt);
						data[4 * ((height - k) * width + (j - 1))] = color.r;
						data[4 * ((height - k) * width + (j - 1)) + 1] = color.g;
						data[4 * ((height - k) * width + (j - 1)) + 2] = color.b;
						data[4 * ((height - k) * width + (j - 1)) + 3] = color.a;
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

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(i / mGrid.mU.mMax[2], 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_YZ);
				shader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);

			}
		}

		void Renderer::drawXZSheets()
		{

			for (float j = eps; j <= mGrid.mV.mMax[1]; j += (mGrid.mV.mMax[1] - 2 * eps) / (MAC3dPara::xzSheetsNum - 1)) {
				for (int k = height; k >= 1; k--) {
					for (int i = width; i >= 1; i--) {
						float pt_x = i * mGrid.mU.mMax[0] / (width);
						float pt_y = j;
						float pt_z = k * mGrid.mW.mMax[2] / (height);
						glm::vec3 pt(pt_x, pt_y, pt_z);
						glm::vec4 color = mGrid.getRenderColor(pt);
						data[4 * ((height - k) * width + (width - i))] = color.r;
						data[4 * ((height - k) * width + (width - i)) + 1] = color.g;
						data[4 * ((height - k) * width + (width - i)) + 2] = color.b;
						data[4 * ((height - k) * width + (width - i)) + 3] = color.a;
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

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, j / mGrid.mV.mMax[1], 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(shader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_XZ);
				shader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);

			}
		}

		GLuint Renderer::getTextureID() {
			return textureID;
		}

	}
}