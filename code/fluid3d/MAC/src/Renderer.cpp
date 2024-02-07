#include "Renderer.h"
#include "fluid3d/MAC/include/Renderer.h"

namespace FluidSimulation {
	namespace MAC3d {
		void Renderer::init()
		{

		}

		void Renderer::createTextureFromFramebuffer()
		{
			GLuint frameBuffer;
			glGenFramebuffers(1, &frameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << "Framebuffer is not complete!" << std::endl;
				return;
			}
		}

		void Renderer::draw() {
			/*createTextureFromFramebuffer();
			

			mCamera.updateOpenGLMatrices();
			drawXSheets();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
			drawOneSheet();
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
			backToFront = mCamera.GetPosition()[0] < 0;

			// Draw Sheets from back to front
			double back = (MAC3dPara::theDim3d[2]) * MAC3dPara::theCellSize3d;
			double top = (MAC3dPara::theDim3d[1]) * MAC3dPara::theCellSize3d;
			double right = (MAC3dPara::theDim3d[0]) * MAC3dPara::theCellSize3d;

			double stepSize = right / (MAC3dPara::sheetsNum + 1);

			double starti = right - stepSize;
			double endi = 0;
			double stepi = -stepSize;

			if (!backToFront)
			{
				starti = 0;
				endi = right;
				stepi = stepSize;
			}

			for (double i = starti; backToFront ? i > endi : i < endi; i += stepi)
			{
				for (double j = 0.0; j < top; )
				{
					glBegin(GL_QUAD_STRIP);
					for (double k = 0.0; k <= back; k += stepSize)
					{
						glm::vec3 pos1 = glm::vec3(i, j, k);
						glm::vec3 pos2 = glm::vec3(i, j + stepSize, k);

						glm::vec4 color1 = mGrid.getRenderColor(pos1);
						glm::vec4 color2 = mGrid.getRenderColor(pos2);

						double c1[4] = { color1[0], color1[1], color1[2], color1[3] };
						double p1[4] = { pos1[0], pos1[1], pos1[2], pos1[3] };
						glColor4dv(c1);
						glVertex3dv(p1);
						double c2[4] = { color2[0], color2[1], color2[2], color2[3] };
						double p2[4] = { pos2[0], pos2[1], pos2[2], pos2[3] };
						glColor4dv(c2);
						glVertex3dv(p2);
					}
					glEnd();
					j += stepSize;

					glBegin(GL_QUAD_STRIP);
					for (double k = back; k >= 0.0; k -= stepSize)
					{
						glm::vec3 pos1 = glm::vec3(i, j, k);
						glm::vec3 pos2 = glm::vec3(i, j + stepSize, k);

						glm::vec4 color1 = mGrid.getRenderColor(pos1);
						glm::vec4 color2 = mGrid.getRenderColor(pos2);
						double c1[4] = { color1[0], color1[1], color1[2], color1[3] };
						double p1[4] = { pos1[0], pos1[1], pos1[2], pos1[3] };
						glColor4dv(c1);
						glVertex3dv(p1);
						double c2[4] = { color2[0], color2[1], color2[2], color2[3] };
						double p2[4] = { pos2[0], pos2[1], pos2[2], pos2[3] };
						glColor4dv(c2);
						glVertex3dv(p2);
					}
					glEnd();
					j += stepSize;
				}
			}
		}

		GLuint Renderer::getImTextureIDByDensity() {
			return textureID;
		}

	}
}