#include "InspectorView.h"


namespace FluidSimulation {
	InspectorView::InspectorView() {

	}

	InspectorView::InspectorView(GLFWwindow* window) {
		this->window = window;
		showID = false;
	}

	void InspectorView::display() {
		ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoCollapse);

		if (ImGui::BeginCombo("methods", Manager::GetInstance().GetSceneView()->currentMethod == NULL ? NULL : Manager::GetInstance().GetSceneView()->currentMethod->description))
		{
			for (int i = 0; i < methodComponents.size(); i++)
			{
				bool is_selected = (Manager::GetInstance().GetSceneView()->currentMethod == methodComponents[i]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(methodComponents[i]->description, is_selected)) {
					if (Manager::GetInstance().GetSceneView()->currentMethod != methodComponents[i]) {
						if (Manager::GetInstance().GetSceneView()->currentMethod != NULL) {
							Manager::GetInstance().GetSceneView()->currentMethod->shutDown();  // 释放原来的模拟方法占用的内存
						}
						Manager::GetInstance().GetSceneView()->currentMethod = methodComponents[i];
						Manager::GetInstance().GetSceneView()->currentMethod->init();
						Manager::GetInstance().GetSceneView()->texture = -1;
					}
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}

		ImGui::SetNextItemWidth(300);
		if (ImGui::Button(Manager::GetInstance().GetSceneView()->isRendering ? "Pause" : "Continue")) {
			// 切换渲染状态
			Manager::GetInstance().GetSceneView()->isRendering = !Manager::GetInstance().GetSceneView()->isRendering;
			if (Manager::GetInstance().GetSceneView()->isRendering) {
				Manager::GetInstance().pushMessage("Rendering...");
			}
			else {
				Manager::GetInstance().pushMessage("Stopped.");
			}
		}

		if (ImGui::Button("Reset")) {
			glfwMakeContextCurrent(window);

			Manager::GetInstance().GetSceneView()->currentMethod->init();

			Manager::GetInstance().GetSceneView()->isRendering = false;
			Manager::GetInstance().GetSceneView()->texture = -1;

			Manager::GetInstance().pushMessage("Reset succeeded.");
		}



		if (Manager::GetInstance().GetSceneView()->currentMethod==NULL) {
			ImGui::Text("No Simulation Method!\nPlease select a simulation method.");
		}
		else{
			int intStep = 1;
			float floatStep = 0.1;
			switch (Manager::GetInstance().GetSceneView()->currentMethod->id)
			{

			// sph 3d
			case 2:
				ImGui::Text("Camera:");
				ImGui::InputFloat3("Position", &Manager::GetInstance().GetSceneView()->currentMethod->camera->mPosition.x);
				ImGui::InputScalar("Fov", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->fovyDeg, &floatStep, NULL);
				ImGui::InputScalar("Aspect", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->aspect, &floatStep, NULL);
				ImGui::InputScalar("Near", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->nearPlane, &floatStep, NULL);
				ImGui::InputScalar("Far", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->farPlane, &floatStep, NULL);
				ImGui::InputScalar("Yaw", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->mYaw, &floatStep, NULL);
				ImGui::InputScalar("Pitch", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->mPitch, &floatStep, NULL);
				Manager::GetInstance().GetSceneView()->currentMethod->camera->UpdateView();

				break;
			// mac 3d
			case 3:
				ImGui::Text("Camera:");
				ImGui::InputFloat3("Position", &Manager::GetInstance().GetSceneView()->currentMethod->camera->mPosition.x);
				ImGui::InputScalar("Fov", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->fovyDeg, &floatStep, NULL);
				ImGui::InputScalar("Aspect", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->aspect, &floatStep, NULL);
				ImGui::InputScalar("Near", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->nearPlane, &floatStep, NULL);
				ImGui::InputScalar("Far", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->farPlane, &floatStep, NULL);
				ImGui::InputScalar("Yaw", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->mYaw, &floatStep, NULL);
				ImGui::InputScalar("Pitch", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->mPitch, &floatStep, NULL);
				Manager::GetInstance().GetSceneView()->currentMethod->camera->UpdateView();

				ImGui::Separator();

				ImGui::Text("Rendering:");
				ImGui::Checkbox("X-Y", &MAC3dPara::xySheetsON);
				ImGui::Checkbox("Y-Z", &MAC3dPara::yzSheetsON);
				ImGui::Checkbox("X-Z", &MAC3dPara::xzSheetsON);
				ImGui::InputScalar("X-Y Sheets", ImGuiDataType_S32, &MAC3dPara::xySheetsNum, &intStep, NULL);
				ImGui::InputScalar("Y-Z Sheets", ImGuiDataType_S32, &MAC3dPara::yzSheetsNum, &intStep, NULL);
				ImGui::InputScalar("X-Z Sheets", ImGuiDataType_S32, &MAC3dPara::xzSheetsNum, &intStep, NULL);

				ImGui::Separator();

			}
		}

		ImGui::End();
	}
}