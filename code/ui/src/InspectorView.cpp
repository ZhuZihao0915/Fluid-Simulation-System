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
		if (ImGui::Button(simulating ? "Pause" : "Continue")) {
			// 切换渲染状态
			simulating = !simulating;
			if (simulating) {
				Manager::GetInstance().pushMessage("Rendering...");
			}
			else {
				Manager::GetInstance().pushMessage("Stopped.");
			}
		}

		if (ImGui::Button("Reset")) {
			glfwMakeContextCurrent(window);

			Manager::GetInstance().GetSceneView()->currentMethod->init();

			simulating = false;
			Manager::GetInstance().GetSceneView()->texture = -1;

			Manager::GetInstance().pushMessage("Reset succeeded.");
		}

		ImGui::Separator();

		if (Manager::GetInstance().GetSceneView()->currentMethod==NULL) {
			ImGui::Text("No Simulation Method!\nPlease select a simulation method.");
		}
		else{
			int intStep = 1;
			float floatStep = 0.1;
			switch (Manager::GetInstance().GetSceneView()->currentMethod->id)
			{
			// TODO
			// 添加各算法的参数控制组件
			
			// sph 2d
			case 0:

				ImGui::Text("Physical Parameters:");
				ImGui::SliderFloat("Gravity", &SPH2dPara::gravity, -10.0f, 20.0f);
				ImGui::SliderFloat("Density", &SPH2dPara::density, 100.0f, 2000.0f);
				ImGui::SliderFloat("Stiffness", &SPH2dPara::stiffness, 10.0f, 100.0f);
				ImGui::SliderFloat("Viscosity", &SPH2dPara::viscosity, 0.0f, 0.5f);

				ImGui::Separator();

				ImGui::Text("Solver:");
				ImGui::SliderFloat("Delta Time", &SPH2dPara::dt, 0.0f, 6e-2f);
				ImGui::InputScalar("Substep", ImGuiDataType_S32, &SPH2dPara::substep, &intStep, NULL);
				

				break;
			// mac 2d
			case 1:
				ImGui::Text("Physical Parameters:");
				ImGui::SliderFloat("Air Density", &MAC2dPara::airDensity, 0.10f, 3.0f);
				ImGui::SliderFloat("Ambient Temperature", &MAC2dPara::ambientTemp, 0.0f, 50.0f);
				ImGui::SliderFloat("Boussinesq Alpha", &MAC2dPara::boussinesqAlpha, 100.0f, 1000.0f);
				ImGui::SliderFloat("Boussinesq Beta", &MAC2dPara::boussinesqBeta, 1000.0f, 5000.0f);
				ImGui::SliderFloat("Vorticity", &MAC2dPara::vorticityConst, 10.0f, 200.0f);
				
				ImGui::Separator();

				ImGui::Text("Solver:");
				ImGui::SliderFloat("Delta Time", &MAC2dPara::dt, 0.0f, 0.1f);
				ImGui::SliderFloat("Source Velocity", &MAC2dPara::sourceVelocity, 0.0f, 5.0f);

				break;
			// sph 3d
			case 2:
				ImGui::Text("Camera:");
				ImGui::InputFloat3("Position", &Glb::Camera::getInstance().mPosition.x);
				ImGui::InputScalar("Fov", ImGuiDataType_Float, &Glb::Camera::getInstance().fovyDeg, &floatStep, NULL);
				ImGui::InputScalar("Aspect", ImGuiDataType_Float, &Glb::Camera::getInstance().aspect, &floatStep, NULL);
				ImGui::InputScalar("Near", ImGuiDataType_Float, &Glb::Camera::getInstance().nearPlane, &floatStep, NULL);
				ImGui::InputScalar("Far", ImGuiDataType_Float, &Glb::Camera::getInstance().farPlane, &floatStep, NULL);
				ImGui::InputScalar("Yaw", ImGuiDataType_Float, &Glb::Camera::getInstance().mYaw, &floatStep, NULL);
				ImGui::InputScalar("Pitch", ImGuiDataType_Float, &Glb::Camera::getInstance().mPitch, &floatStep, NULL);
				Glb::Camera::getInstance().UpdateView();

				ImGui::Separator();

				ImGui::Text("Solver:");
				ImGui::SliderFloat("Delta Time", &SPH3dPara::dt, 0.0f, 6e-2f);
				ImGui::InputScalar("Substep", ImGuiDataType_S32, &SPH3dPara::substep, &intStep, NULL);
				ImGui::InputScalar("Velocity Attenuation", ImGuiDataType_Float, &SPH3dPara::velocityAttenuation, &floatStep, NULL);

				ImGui::Separator();

				ImGui::Text("Physical Parameters:");
				ImGui::SliderFloat("Gravity", &SPH3dPara::gravity, -10.0f, 20.0f);
				ImGui::SliderFloat("Density", &SPH3dPara::density0, 100.0f, 2000.0f);
				ImGui::SliderFloat("Stiffness", &SPH3dPara::stiffness, 10.0f, 50.0f);
				ImGui::SliderFloat("Viscosity", &SPH3dPara::viscosity, 0.0f, 8e-3f);

				break;
			// mac 3d
			case 3:
				ImGui::Text("Camera:");
				ImGui::InputFloat3("Position", &Glb::Camera::getInstance().mPosition.x);
				ImGui::InputScalar("Fov", ImGuiDataType_Float, &Glb::Camera::getInstance().fovyDeg, &floatStep, NULL);
				ImGui::InputScalar("Aspect", ImGuiDataType_Float, &Glb::Camera::getInstance().aspect, &floatStep, NULL);
				ImGui::InputScalar("Near", ImGuiDataType_Float, &Glb::Camera::getInstance().nearPlane, &floatStep, NULL);
				ImGui::InputScalar("Far", ImGuiDataType_Float, &Glb::Camera::getInstance().farPlane, &floatStep, NULL);
				ImGui::InputScalar("Yaw", ImGuiDataType_Float, &Glb::Camera::getInstance().mYaw, &floatStep, NULL);
				ImGui::InputScalar("Pitch", ImGuiDataType_Float, &Glb::Camera::getInstance().mPitch, &floatStep, NULL);
				Glb::Camera::getInstance().UpdateView();

				ImGui::Separator();

				ImGui::Text("Rendering:");
				ImGui::Checkbox("One Sheet", &MAC3dPara::oneSheet);
				ImGui::Checkbox("X-Y", &MAC3dPara::xySheetsON);
				ImGui::Checkbox("Y-Z", &MAC3dPara::yzSheetsON);
				ImGui::Checkbox("X-Z", &MAC3dPara::xzSheetsON);
				if (MAC3dPara::oneSheet) {
					ImGui::SliderFloat("Distance", &MAC3dPara::distance, 0.0f, 1.0f);
				}
				else {
					ImGui::InputScalar("X-Y Sheets", ImGuiDataType_S32, &MAC3dPara::xySheetsNum, &intStep, NULL);
					ImGui::InputScalar("Y-Z Sheets", ImGuiDataType_S32, &MAC3dPara::yzSheetsNum, &intStep, NULL);
					ImGui::InputScalar("X-Z Sheets", ImGuiDataType_S32, &MAC3dPara::xzSheetsNum, &intStep, NULL);
				}

				ImGui::Separator();

				ImGui::Text("Solver:");
				ImGui::SliderFloat("Delta Time", &MAC3dPara::dt, 0.0f, 0.1f);
				ImGui::SliderFloat("Source Velocity", &MAC3dPara::sourceVelocity, 0.0f, 5.0f);

				ImGui::Separator();

				ImGui::Text("Physical Parameters:");
				ImGui::SliderFloat("Air Density", &MAC3dPara::airDensity, 0.10f, 3.0f);
				ImGui::SliderFloat("Ambient Temperature", &MAC3dPara::ambientTemp, 0.0f, 50.0f);
				ImGui::SliderFloat("Boussinesq Alpha", &MAC3dPara::boussinesqAlpha, 100.0f, 1000.0f);
				ImGui::SliderFloat("Boussinesq Beta", &MAC3dPara::boussinesqBeta, 1000.0f, 5000.0f);
				ImGui::SliderFloat("Vorticity", &MAC3dPara::vorticityConst, 10.0f, 200.0f);
				break;

			}
		}

		ImGui::End();
	}
}