#include "MAC/include/Renderer.h"

namespace FluidSimulation {
	namespace MAC2d {

		GLuint Renderer::getImTextureIDByDensity() {
			
			std::vector<float> imageData;

			for (int j = 1; j <= imageHeight; j++) {
				for (int i = 1; i <= imageWidth; i++) {
					float pt_x = i * mGrid->mU.mMax[0] / (imageWidth);
					float pt_y = j * mGrid->mU.mMax[1] / (imageHeight);
					glm::vec2 pt(pt_x, pt_y);
					glm::vec4 color = mGrid->getRenderColor(pt);
					imageData.push_back(color.x);
					imageData.push_back(color.y);
					imageData.push_back(color.z);
				}
			}
			applyFilter(imageData, imageWidth, imageHeight);

			GLuint textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			// �����������
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// ����ɫ���ݴ��ݸ�����
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_FLOAT, imageData.data());
			glBindTexture(GL_TEXTURE_2D, 0);
			return textureID;

		}

		void Renderer::applyFilter(std::vector<float>& imageData, int width, int height) {
			// ����һ����ʱ�����洢�˲���Ľ��
			std::vector<float> resultData(imageData.size(), 0.0f);

			// ������˹�˲�����
			int laplacianKernel[3][3] = { {0, -1, 0}, {-1, 4, -1}, {0, -1, 0} };

			// ����ͼ���е�ÿ������
			for (int y = 1; y < height - 1; ++y) {
				for (int x = 1; x < width - 1; ++x) {
					for (int c = 0; c < 3; ++c) { // ����ÿ����ɫͨ��
						float laplacianValue = 0.0f;

						// ��3x3�Ĵ�����Ӧ��������˹�˲���
						for (int i = -1; i <= 1; ++i) {
							for (int j = -1; j <= 1; ++j) {
								int index = ((y + i) * width + (x + j)) * 3 + c; // ������ɫͨ��
								laplacianValue += imageData[index] * laplacianKernel[i + 1][j + 1];
							}
						}

						// ������洢����ʱ������
						resultData[(y * width + x) * 3 + c] = imageData[(y * width + x) * 3 + c] + laplacianValue;
					}
				}
			}

			// ��������ƻ�ԭʼͼ������
			imageData = resultData;
		}

	}
}