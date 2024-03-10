#include "stb_image.h"
#include "fluid3d/MAC/include/Renderer.h"

namespace FluidSimulation {
	namespace MAC3d {

		float vertices[]{
			//position		  //texcood   //color
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
		};

		unsigned int indices[] = {
			0, 1, 2, // first triangle
			0, 2, 3  // second triangle
		};

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

			std::string particalVertShaderPath = shaderPath + "/DrawSmokePixel3d.vert";
			std::string particalFragShaderPath = shaderPath + "/DrawSmokePixel3d.frag";
			pixelShader = new Glb::Shader();
			pixelShader->buildFromFile(particalVertShaderPath, particalFragShaderPath);

			particalVertShaderPath = shaderPath + "/DrawSmokeTexture3d.vert";
			particalFragShaderPath = shaderPath + "/DrawSmokeTexture3d.frag";
			gridShader = new Glb::Shader();
			gridShader->buildFromFile(particalVertShaderPath, particalFragShaderPath);

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glGenVertexArrays(1, &VAO_XY);
			glGenBuffers(1, &VBO_XY);
			glBindVertexArray(VAO_XY);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_XY);
			glBufferData(GL_ARRAY_BUFFER, sizeof(verticesXY), verticesXY, GL_STATIC_DRAW);
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

			loadTexture();

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
				drawOneSheetXY();
			}
			if (MAC3dPara::yzSheetsON)
			{
				drawOneSheetYZ();
			}
			if (MAC3dPara::xzSheetsON) {
				drawOneSheetXZ();
			}
		}

		void Renderer::drawOneSheetXY() {
			if (MAC3dPara::drawModel == 0) {
				for (int j = 1; j <= height; j++) {
					for (int i = 1; i <= width; i++) {
						float pt_x = i * mGrid.mU.mMax[2] / (width);
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

				pixelShader->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glUniform1i(glGetUniformLocation(pixelShader->getId(), "aTexture"), 0);

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, MAC3dPara::distance));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_XY);
				pixelShader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			else if (MAC3dPara::drawModel == 1) {
				for (int j = 1; j <= height; j++) {
					for (int i = 1; i <= width; i++) {
						float pt_x = i * mGrid.mU.mMax[2] / (width);
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
			}
		}

		void Renderer::drawOneSheetXZ() {
			if (MAC3dPara::drawModel == 0) {
				for (int k = height; k >= 1; k--) {
					for (int i = width; i >= 1; i--) {
						float pt_x = i * mGrid.mU.mMax[2] / (width);
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

				pixelShader->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glUniform1i(glGetUniformLocation(pixelShader->getId(), "aTexture"), 0);

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, MAC3dPara::distance, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_XZ);
				pixelShader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			else if (MAC3dPara::drawModel == 1) {

			}
		}

		void Renderer::drawOneSheetYZ() {
			if (MAC3dPara::drawModel == 0) {

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

				pixelShader->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glUniform1i(glGetUniformLocation(pixelShader->getId(), "aTexture"), 0);

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(MAC3dPara::distance * mGrid.mU.mMax[0] / mGrid.mW.mMax[2], 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_YZ);
				pixelShader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			else if (MAC3dPara::drawModel == 1) {

				float dt_y = mGrid.mD.mMax[1] / (MAC3dPara::gridNumY);
				float dt_z = mGrid.mD.mMax[2] / (MAC3dPara::gridNumZ);

				for (int k = MAC3dPara::gridNumZ; k >= 1; k--) {
					for (int j = 1; j <= MAC3dPara::gridNumY; j++) {
						float pt_x = MAC3dPara::distance * mGrid.mD.mMax[0];
						float pt_y = j * mGrid.mD.mMax[1] / (MAC3dPara::gridNumY);
						float pt_z = k * mGrid.mD.mMax[2] / (MAC3dPara::gridNumZ);

						vertices[1] = pt_y - dt_y / 2;
						vertices[2] = pt_z - dt_z / 2;
						vertices[5] = mGrid.getDensity(glm::vec3(pt_x, vertices[1], vertices[2]));

						vertices[7] = pt_y + dt_y / 2;
						vertices[8] = pt_z - dt_z / 2;
						vertices[11] = mGrid.getDensity(glm::vec3(pt_x, vertices[1], vertices[2]));

						vertices[13] = pt_y + dt_y / 2;
						vertices[14] = pt_z + dt_z / 2;
						vertices[17] = mGrid.getDensity(glm::vec3(pt_x, vertices[1], vertices[2]));

						vertices[19] = pt_y - dt_y / 2;
						vertices[20] = pt_z + dt_z / 2;
						vertices[23] = mGrid.getDensity(glm::vec3(pt_x, vertices[1], vertices[2]));

						for (int k = 1; k <= 19; k += 6) {
							vertices[k] = (vertices[k] / mGrid.mD.mMax[1]) * 2 - 1;
							vertices[k + 1] = (vertices[k + 1] / mGrid.mD.mMax[2]) * 2 - 1;
						}

						glBindVertexArray(VAO);
						glBindBuffer(GL_ARRAY_BUFFER, VBO);
						glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
						glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
						glEnableVertexAttribArray(0);
						glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
						glEnableVertexAttribArray(1);
						glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
						glEnableVertexAttribArray(2);
						glBindVertexArray(0);

						gridShader->use();
						glBindTexture(GL_TEXTURE_2D, smokeTexture);
						glUniform1i(glGetUniformLocation(gridShader->getId(), "mTexture"), 0);

						glm::mat4 view = Glb::Camera::getInstance().GetView();
						glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

						glm::mat4 model = glm::mat4(1.0f);
						model = glm::translate(model, glm::vec3(MAC3dPara::distance * mGrid.mD.mMax[0] / mGrid.mD.mMax[2], 0.0f, 0.0f));
						glUniformMatrix4fv(glGetUniformLocation(gridShader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
						glUniformMatrix4fv(glGetUniformLocation(gridShader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
						glUniformMatrix4fv(glGetUniformLocation(gridShader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

						glBindVertexArray(VAO);
						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


					}
				}
			}
		}

		void Renderer::loadTexture() {
			glGenTextures(1, &smokeTexture);
			glBindTexture(GL_TEXTURE_2D, smokeTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// load image, create texture and generate mipmaps
			int width, height, nrChannels;

			unsigned char* data = stbi_load((picturePath + "/smoke2.png").c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				// 使用图像生成纹理
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				// Mipmaps
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);
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

				pixelShader->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glUniform1i(glGetUniformLocation(pixelShader->getId(), "aTexture"), 0);

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, k/ mGrid.mW.mMax[2]));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_XY);
				pixelShader->use();
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

				pixelShader->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glUniform1i(glGetUniformLocation(pixelShader->getId(), "aTexture"), 0);

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(i / mGrid.mU.mMax[2], 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_YZ);
				pixelShader->use();
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

				pixelShader->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glUniform1i(glGetUniformLocation(pixelShader->getId(), "aTexture"), 0);

				glm::mat4 view = Glb::Camera::getInstance().GetView();
				glm::mat4 projection = Glb::Camera::getInstance().GetProjection();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, j / mGrid.mV.mMax[1], 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(glGetUniformLocation(pixelShader->getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(VAO_XZ);
				pixelShader->use();
				glDrawArrays(GL_TRIANGLES, 0, 6);

			}
		}

		GLuint Renderer::getTextureID() {
			return textureID;
		}

	}
}