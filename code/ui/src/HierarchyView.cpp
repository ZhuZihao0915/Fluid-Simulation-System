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

	void HierarchyView::display2d() {
		int windowPosX, windowPosY;
		glfwGetWindowPos(window, &windowPosX, &windowPosY);

		// ���� ImGui ���ڵ�λ�ã�ʹ�����Ͻ��� GLFW ���ڵ����ϽǶ���
		//ImGui::SetNextWindowPos(ImVec2(static_cast<float>(windowPosX), static_cast<float>(windowPosY)), ImGuiCond_FirstUseEver);
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		//ImGui::SetNextWindowSize(ImVec2(width*0.1, height*0.1), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);
		ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoCollapse);


		bool itemSelected = false;

		if (ImGui::Selectable("Fluid", &itemSelected)) {
			// �������¼�
			// itemSelected ���ڱ����˸����ѡ��״̬
			if (itemSelected) {
				// �ѡ��ʱִ�еĴ���
				Manager::GetInstance().GetInspectorView()->show2dID = 1;
			}
			else {
				// ��ȡ��ѡ��ʱִ�еĴ���
				Manager::GetInstance().GetInspectorView()->show2dID = false;
			}
		}

		bool itemSelected2 = false;
		if (ImGui::Selectable("Container", &itemSelected2)) {
			if (itemSelected2) { Manager::GetInstance().GetInspectorView()->show2dID = 2; }
			else { Manager::GetInstance().GetInspectorView()->show2dID = false; }
		}
		bool itemSelected3 = false;
		if (ImGui::Selectable("Iteration", &itemSelected3)) {
			if (itemSelected3) { Manager::GetInstance().GetInspectorView()->show2dID = 3; }
			else { Manager::GetInstance().GetInspectorView()->show2dID = false; }
		}

		/*
		if (ImGui::TreeNodeEx("Fluid", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
			if (ImGui::IsItemClicked()) {
				Manager::GetInstance().GetInspectorView()->show = true;
			}
			ImGui::TreePop();
		}
		*/

		ImGui::End();
	}

	void HierarchyView::display3d() {

	}
}