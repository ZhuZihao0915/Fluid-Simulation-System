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

	void SceneView::display2d() {
		ImGui::Begin("Scene", NULL, ImGuiWindowFlags_NoCollapse);

        ImVec2 windowSize = ImGui::GetWindowSize();

        ImGui::SetNextItemWidth(100);

        if (ImGui::BeginCombo("methods", currentMethod == NULL ? NULL : currentMethod->description))
        {
            for (int i = 0; i < methodComponents.size(); i++)
            {
                bool is_selected = (currentMethod == methodComponents[i]); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(methodComponents[i]->description, is_selected)) {
                    if (currentMethod != methodComponents[i]) {
                        if (currentMethod != NULL) {
                            currentMethod->shutDown();  // �ͷ�ԭ����ģ�ⷽ��ռ�õ��ڴ�
                        }
                        currentMethod = methodComponents[i];
                        currentMethod->init();
                        texture = -1;
                    }
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
            ImGui::EndCombo();
        }
        


        ImGui::SetCursorPosX((windowSize.x - 200) * 0.5f);
        ImGui::SetCursorPosY(50);

        ImGui::SetNextItemWidth(300);
        if (ImGui::Button(isRendering ? "Pause" : "Continue")) {
            // �л���Ⱦ״̬
            isRendering = !isRendering;
            if (isRendering) {
                Manager::GetInstance().pushMessage("Rendering...");
            }
            else {
                Manager::GetInstance().pushMessage("Stopped.");
            }
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX((windowSize.x + 40) * 0.5f);
        if (ImGui::Button("Reset")) {
            glfwMakeContextCurrent(window);
            
            currentMethod->init();

            isRendering = false;
            texture = -1;

            Manager::GetInstance().pushMessage("Reset succeeded.");
        }
        
        // run sph when rendering 
        // or haven't started (don't have a picture to show) , just get one
        if (currentMethod != NULL && (isRendering || texture == -1)) {

            currentMethod->simulate();

            texture = currentMethod->getRenderedTexture();

        }

        // show image
        ImGui::SetCursorPosX((windowSize.x - 600) * 0.5f);
        ImGui::SetCursorPosY(90);

        ImVec2 imageSize(600, 600);
        if (currentMethod == NULL) {
            ImGui::Image(NULL, imageSize);
        }
        else { 
            ImGui::Image((ImTextureID)texture, imageSize); 
        }
        ImGui::GetWindowDrawList()->AddImage((ImTextureID)texture,
            ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y), ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y),
            ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
	}


    void SceneView::display3d() {
        
    }
}