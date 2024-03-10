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

		ImGui::PushItemWidth(200);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20.0f, 7.0f));

		ImGui::Text("Simulation Method:");
		if (ImGui::BeginCombo("methods", Manager::getInstance().getMethod() == NULL ? NULL : Manager::getInstance().getMethod()->description))
		{
			for (int i = 0; i < methodComponents.size(); i++)
			{
				bool is_selected = (Manager::getInstance().getMethod() == methodComponents[i]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(methodComponents[i]->description, is_selected)) {
					if (Manager::getInstance().getMethod() != methodComponents[i]) {
						if (Manager::getInstance().getMethod() != NULL) {
							Manager::getInstance().getMethod()->shutDown();  // 释放原来的模拟方法占用的内存
						}
						Manager::getInstance().setMethod(methodComponents[i]);
						Manager::getInstance().getMethod()->init();
						Manager::getInstance().getSceneView()->texture = -1;
					}
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}


		ImGui::SetNextItemWidth(300);
		if (ImGui::Button(simulating ? "Stop" : "Continue")) {
			// 切换渲染状态
			simulating = !simulating;
			if (simulating) {
				Manager::getInstance().pushMessage("Rendering...");
			}
			else {
				Manager::getInstance().pushMessage("Stopped.");
			}
		}

		if (ImGui::Button("Rerun")) {
			glfwMakeContextCurrent(window);

			Manager::getInstance().getMethod()->init();

			simulating = false;
			Manager::getInstance().getSceneView()->texture = -1;

			Manager::getInstance().pushMessage("Rerun succeeded.");
		}

		ImGui::Separator();

		if (Manager::getInstance().getMethod() ==NULL) {
			ImGui::Text("Please select a simulation method.");
		}
		else{
			int intStep = 1;
			float floatStep = 0.1;
			double doubleStep = 0.0001;
			float fontSize = ImGui::GetFontSize();

			// 设置新字体大小
			
			switch (Manager::getInstance().getMethod()->id)
			{
			// TODO
			// 添加各算法的参数控制组件
			
			// sph 2d
			case 0:
				ImGui::Text("Physical Parameters:");
				ImGui::PushItemWidth(200);
				ImGui::SliderFloat("Gravity.X", &SPH2dPara::gravityX, -10.0f, 20.0f);
				ImGui::SliderFloat("Gravity.Y", &SPH2dPara::gravityY, -10.0f, 20.0f);
				ImGui::SliderFloat("Density", &SPH2dPara::density, 500.0f, 1500.0f);
				ImGui::SliderFloat("Stiffness", &SPH2dPara::stiffness, 10.0f, 100.0f);
				ImGui::SliderFloat("Viscosity", &SPH2dPara::viscosity, 0.0f, 1.0f);
				ImGui::PopItemWidth();

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();
				
				ImGui::Text("Solver:");
				ImGui::SliderFloat("Delta Time", &SPH2dPara::dt, 0.0f, 0.003f, "%.5f");
				ImGui::PushItemWidth(150);
				ImGui::InputScalar("Substep", ImGuiDataType_S32, &SPH2dPara::substep, &intStep, NULL);
				ImGui::PopItemWidth();

				break;
			// mac 2d
			case 1:
				ImGui::Text("Physical Parameters:");
				ImGui::PushItemWidth(200);
				ImGui::SliderFloat("Air Density", &MAC2dPara::airDensity, 0.10f, 3.0f);
				ImGui::SliderFloat("Ambient Temperature", &MAC2dPara::ambientTemp, 0.0f, 50.0f);
				ImGui::SliderFloat("Boussinesq Alpha", &MAC2dPara::boussinesqAlpha, 100.0f, 1000.0f);
				ImGui::SliderFloat("Boussinesq Beta", &MAC2dPara::boussinesqBeta, 1000.0f, 5000.0f);
				ImGui::SliderFloat("Vorticity", &MAC2dPara::vorticityConst, 10.0f, 200.0f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text("Solver:");
				ImGui::SliderFloat("Delta Time", &MAC2dPara::dt, 0.0f, 0.1f, "%.5f");
				ImGui::PushItemWidth(150);
				ImGui::SliderFloat("Source Velocity", &MAC2dPara::sourceVelocity, 0.0f, 5.0f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text("Renderer:");
				ImGui::RadioButton("Pixel", &MAC2dPara::drawModel, 0);
				ImGui::RadioButton("Grid", &MAC2dPara::drawModel, 1);
			

				break;
			// sph 3d
			case 2:
				ImGui::Text("Camera:");
				ImGui::PushItemWidth(250);
				ImGui::InputFloat3("Position", &Glb::Camera::getInstance().mPosition.x);
				ImGui::InputScalar("Fov", ImGuiDataType_Float, &Glb::Camera::getInstance().fovyDeg, &floatStep, NULL);
				ImGui::InputScalar("Aspect", ImGuiDataType_Float, &Glb::Camera::getInstance().aspect, &floatStep, NULL);
				ImGui::InputScalar("Near", ImGuiDataType_Float, &Glb::Camera::getInstance().nearPlane, &floatStep, NULL);
				ImGui::InputScalar("Far", ImGuiDataType_Float, &Glb::Camera::getInstance().farPlane, &floatStep, NULL);
				ImGui::InputScalar("Yaw", ImGuiDataType_Float, &Glb::Camera::getInstance().mYaw, &floatStep, NULL);
				ImGui::InputScalar("Pitch", ImGuiDataType_Float, &Glb::Camera::getInstance().mPitch, &floatStep, NULL);
				ImGui::PopItemWidth();
				Glb::Camera::getInstance().UpdateView();

				ImGui::Separator();

				ImGui::Text("Solver:");
				ImGui::SliderFloat("Delta Time", &SPH3dPara::dt, 0.0f, 0.005f, "%.5f");
				ImGui::PushItemWidth(150);
				ImGui::InputScalar("Substep", ImGuiDataType_S32, &SPH3dPara::substep, &intStep, NULL);
				ImGui::InputScalar("Velocity Attenuation", ImGuiDataType_Float, &SPH3dPara::velocityAttenuation, &floatStep, NULL);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text("Physical Parameters:");
				ImGui::SliderFloat("Gravity", &SPH3dPara::gravity, -10.0f, 20.0f);
				ImGui::SliderFloat("Density", &SPH3dPara::density0, 100.0f, 2000.0f);
				ImGui::SliderFloat("Stiffness", &SPH3dPara::stiffness, 10.0f, 50.0f);
				ImGui::SliderFloat("Viscosity", &SPH3dPara::viscosity, 0.0f, 8e-3f, "%.5f");

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
				ImGui::PushItemWidth(150);
				ImGui::SliderFloat("Delta Time", &MAC3dPara::dt, 0.0f, 0.1f, "%.5f");
				ImGui::SliderFloat("Source Velocity", &MAC3dPara::sourceVelocity, 0.0f, 5.0f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text("Physical Parameters:");
				ImGui::SliderFloat("Air Density", &MAC3dPara::airDensity, 0.10f, 3.0f);
				ImGui::SliderFloat("Ambient Temperature", &MAC3dPara::ambientTemp, 0.0f, 50.0f);
				ImGui::SliderFloat("Boussinesq Alpha", &MAC3dPara::boussinesqAlpha, 100.0f, 1000.0f);
				ImGui::SliderFloat("Boussinesq Beta", &MAC3dPara::boussinesqBeta, 1000.0f, 5000.0f);
				ImGui::SliderFloat("Vorticity", &MAC3dPara::vorticityConst, 10.0f, 200.0f);
				break;

			}
			
			//ImGui::GetStyle().ScaleAllSizes(2.0f);
			if (!Glb::Timer::getInstance().empty()) {
				ImGui::Separator();
				ImGui::Text("Timing:");
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
				ImGui::Text(Glb::Timer::getInstance().currentStatus().c_str());
				ImGui::PopStyleColor();
			}
		}

		ImGui::PopStyleVar();
		ImGui::PopItemWidth();

		ImGui::End();
	}
}