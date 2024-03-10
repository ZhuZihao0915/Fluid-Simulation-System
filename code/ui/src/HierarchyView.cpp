#include "HierarchyView.h"

namespace FluidSimulation {
	HierarchyView::HierarchyView() {
		this->window = NULL;
		pos = ImVec2(0.0f, 0.0f);
	}

	HierarchyView::HierarchyView(GLFWwindow* window) {
		this->window = window;
		pos = ImVec2(0.0f, 0.0f);
	}

	void HierarchyView::display() {
		int windowPosX, windowPosY;
		glfwGetWindowPos(window, &windowPosX, &windowPosY);

		// 设置 ImGui 窗口的位置，使其左上角与 GLFW 窗口的左上角对齐
		//ImGui::SetNextWindowPos(ImVec2(static_cast<float>(windowPosX), static_cast<float>(windowPosY)), ImGuiCond_FirstUseEver);
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		//ImGui::SetNextWindowSize(ImVec2(width*0.1, height*0.1), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);
		ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoCollapse);


		if (Manager::getInstance().getMethod() == NULL) {
			ImGui::Text("No Simulation Method!\nPlease select a simulation method.");
		}
		else {
			switch (Manager::getInstance().getMethod()->id)
			{
				// mac 3d
			case 3:
				bool itemSelected0 = false;
				if (ImGui::Selectable("Camera", &itemSelected0)) {
					if (itemSelected0) { Manager::getInstance().getInspectorView()->showID = 0; }
					else { Manager::getInstance().getInspectorView()->showID = false; }
				}

				bool itemSelected1 = false;
				if (ImGui::Selectable("Fluid", &itemSelected1)) {
					if (itemSelected1) { Manager::getInstance().getInspectorView()->showID = 1; }
					else { Manager::getInstance().getInspectorView()->showID = false; }
				}

				bool itemSelected2 = false;
				if (ImGui::Selectable("Container", &itemSelected2)) {
					if (itemSelected2) { Manager::getInstance().getInspectorView()->showID = 2; }
					else { Manager::getInstance().getInspectorView()->showID = false; }
				}

				bool itemSelected3 = false;
				if (ImGui::Selectable("Iteration", &itemSelected3)) {
					if (itemSelected3) { Manager::getInstance().getInspectorView()->showID = 3; }
					else { Manager::getInstance().getInspectorView()->showID = false; }
				}
			}
		}

		ImGui::End();
	}
}