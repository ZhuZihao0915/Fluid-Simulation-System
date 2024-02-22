#include "SceneView.h"

namespace FluidSimulation {

	SceneView::SceneView() {

	}

	SceneView::SceneView(GLFWwindow* window) {
		this->window = window;
        glfwMakeContextCurrent(window);
        currentMethod = NULL;
        isRendering = false;
        texture = -1;   // no texture;
	}

	void SceneView::display() {
        //ImGui::SetNextWindowSizeConstraints(ImVec2(900, 900), ImVec2(1200, 1200));
		ImGui::Begin("Scene", NULL, ImGuiWindowFlags_NoCollapse);

        ImVec2 windowSize = ImGui::GetWindowSize();



        // run sph when rendering 
        // or haven't started (don't have a picture to show) , just get one
        if (currentMethod != NULL && (isRendering || texture == -1)) {

            currentMethod->simulate();

            texture = currentMethod->getRenderedTexture();

        }

        // show image
        ImGui::SetCursorPosX((windowSize.x - imageWidth) * 0.5f);
        ImGui::SetCursorPosY(50);

        ImVec2 imageSize(imageWidth, imageHeight);
        if (currentMethod == NULL) {
            ImGui::Image(NULL, imageSize);
        }
        else { 
            ImGui::Image((ImTextureID)texture, imageSize); 
        }
        ImGui::GetWindowDrawList()->AddImage((ImTextureID)texture,
            ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y), ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y),
            ImVec2(0, 1), ImVec2(1, 0));

        // 如果鼠标停留在渲染界面上，将鼠标的运动报告给component
        if (ImGui::IsItemHovered() && currentMethod != NULL) {

            // 获取鼠标状态
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // 检测滚轮
            currentMethod->cameraScale(ImGui::GetIO().MouseWheel);

            // 检测鼠标左键拖动
            if (ImGui::IsMouseDragging(0, 0.0f)) {
                if (!isLeftMouseDragging) {
                    isLeftMouseDragging = true;
                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                }
                float deltaX = (float)(mouseX - lastMouseX);
                float deltaY = (float)(mouseY - lastMouseY);

                currentMethod->cameraRotate(deltaX, deltaY);
                
                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }
            else {
                isLeftMouseDragging = false;
            }

            // 检测鼠标右键拖动
            if (ImGui::IsMouseDragging(1, 0.0f)) {
                if (!isRightMouseDragging) {
                    isRightMouseDragging = true;
                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                }
                float deltaX = (float)(mouseX - lastMouseX);
                float deltaY = (float)(mouseY - lastMouseY);

                currentMethod->cameraMove(deltaX, deltaY);

                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }
            else {
                isRightMouseDragging = false;
            }

        }

		ImGui::End();
	}
}