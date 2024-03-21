#include "InspectorView.h"

namespace FluidSimulation
{
	InspectorView::InspectorView()
	{

	}

	InspectorView::InspectorView(GLFWwindow *window)
	{
		this->window = window;
		showID = false;
	}

	void InspectorView::display()
	{
		ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoCollapse);

		ImGui::PushItemWidth(200);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20.0f, 7.0f));

		ImGui::Text("Simulation Method:");
		if (ImGui::BeginCombo("methods", Manager::getInstance().getMethod() == NULL ? NULL : Manager::getInstance().getMethod()->description))
		{
			for (int i = 0; i < methodComponents.size(); i++)
			{
				bool is_selected = (Manager::getInstance().getMethod() == methodComponents[i]);
				if (ImGui::Selectable(methodComponents[i]->description, is_selected))
				{
					if (Manager::getInstance().getMethod() != methodComponents[i])
					{
						if (Manager::getInstance().getMethod() != NULL)
						{
							Manager::getInstance().getMethod()->shutDown();
						}
						Manager::getInstance().setMethod(methodComponents[i]);
						Manager::getInstance().getMethod()->init();
						Manager::getInstance().getSceneView()->texture = -1;
					}
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::SetNextItemWidth(300);
		if (ImGui::Button(simulating ? "Stop" : "Continue"))
		{
			simulating = !simulating;
			if (simulating)
			{
				Glb::Logger::getInstance().addLog("Rendering...");
			}
			else
			{
				Glb::Logger::getInstance().addLog("Stopped.");
			}
		}

		if (ImGui::Button("Rerun"))
		{
			glfwMakeContextCurrent(window);

			Manager::getInstance().getMethod()->init();

			simulating = false;
			Manager::getInstance().getSceneView()->texture = -1;
			Glb::Logger::getInstance().addLog("Rerun succeeded.");
		}

		ImGui::Separator();

		if (Manager::getInstance().getMethod() == NULL)
		{
			ImGui::Text("Please select a simulation method.");
		}
		else
		{
			int intStep = 1;
			float floatStep = 0.1;
			double doubleStep = 0.0001;

			switch (Manager::getInstance().getMethod()->id)
			{
			// lagrangian 2d
			case 0:
				ImGui::Text("Physical Parameters:");
				ImGui::PushItemWidth(200);
				ImGui::SliderFloat("Gravity.X", &Lagrangian2dPara::gravityX, -20.0f, 20.0f);
				ImGui::SliderFloat("Gravity.Y", &Lagrangian2dPara::gravityY, -20.0f, 20.0f);
				ImGui::SliderFloat("Density", &Lagrangian2dPara::density, 500.0f, 1500.0f);
				ImGui::SliderFloat("Stiffness", &Lagrangian2dPara::stiffness, 10.0f, 100.0f);
				ImGui::SliderFloat("Viscosity", &Lagrangian2dPara::viscosity, 0.0f, 1.0f);
				ImGui::PopItemWidth();

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::Text("Solver:");
				ImGui::SliderFloat("Delta Time", &Lagrangian2dPara::dt, 0.0f, 0.003f, "%.5f");
				ImGui::PushItemWidth(150);
				ImGui::InputScalar("Substep", ImGuiDataType_S32, &Lagrangian2dPara::substep, &intStep, NULL);
				ImGui::PopItemWidth();

				break;
			// eulerian 2d
			case 1:
				ImGui::Text("Physical Parameters:");
				ImGui::PushItemWidth(200);
				ImGui::SliderFloat("Air Density", &Eulerian2dPara::airDensity, 0.10f, 3.0f);
				ImGui::SliderFloat("Ambient Temperature", &Eulerian2dPara::ambientTemp, 0.0f, 50.0f);
				ImGui::SliderFloat("Boussinesq Alpha", &Eulerian2dPara::boussinesqAlpha, 100.0f, 1000.0f);
				ImGui::SliderFloat("Boussinesq Beta", &Eulerian2dPara::boussinesqBeta, 1000.0f, 5000.0f);
				ImGui::SliderFloat("Vorticity", &Eulerian2dPara::vorticityConst, 10.0f, 200.0f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text("Solver:");
				ImGui::SliderFloat("Delta Time", &Eulerian2dPara::dt, 0.0f, 0.1f, "%.5f");
				ImGui::PushItemWidth(150);
				ImGui::SliderFloat("Source Velocity", &Eulerian2dPara::sourceVelocity, 0.0f, 5.0f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text("Renderer:");
				ImGui::RadioButton("Pixel", &Eulerian2dPara::drawModel, 0);
				ImGui::RadioButton("Grid", &Eulerian2dPara::drawModel, 1);
				ImGui::SliderFloat("Contrast", &Eulerian2dPara::contrast, 0.0f, 3.0f);

				break;
			// lagrangian 3d
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
				ImGui::SliderFloat("Delta Time", &Lagrangian3dPara::dt, 0.0f, 0.005f, "%.5f");
				ImGui::PushItemWidth(150);
				ImGui::InputScalar("Substep", ImGuiDataType_S32, &Lagrangian3dPara::substep, &intStep, NULL);
				ImGui::InputScalar("Velocity Attenuation", ImGuiDataType_Float, &Lagrangian3dPara::velocityAttenuation, &floatStep, NULL);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text("Physical Parameters:");
				ImGui::SliderFloat("Gravity.x", &Lagrangian3dPara::gravityX, -20.0f, 20.0f);
				ImGui::SliderFloat("Gravity.y", &Lagrangian3dPara::gravityY, -20.0f, 20.0f);
				ImGui::SliderFloat("Gravity.z", &Lagrangian3dPara::gravityZ, -20.0f, 20.0f);
				ImGui::SliderFloat("Density", &Lagrangian3dPara::density, 100.0f, 2000.0f);
				ImGui::SliderFloat("Stiffness", &Lagrangian3dPara::stiffness, 10.0f, 50.0f);
				ImGui::SliderFloat("Viscosity", &Lagrangian3dPara::viscosity, 0.0f, 0.0006f, "%.5f");

				break;
			// eulerian 3d
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

				ImGui::RadioButton("Pixel", &Eulerian3dPara::drawModel, 0);
				ImGui::RadioButton("Grid", &Eulerian3dPara::drawModel, 1);

				ImGui::SliderFloat("Contrast", &Eulerian3dPara::contrast, 0.0f, 3.0f);

				ImGui::Checkbox("One Sheet", &Eulerian3dPara::oneSheet);
				ImGui::Checkbox("X-Y", &Eulerian3dPara::xySheetsON);
				ImGui::Checkbox("Y-Z", &Eulerian3dPara::yzSheetsON);
				ImGui::Checkbox("X-Z", &Eulerian3dPara::xzSheetsON);
				if (Eulerian3dPara::oneSheet)
				{
					if (Eulerian3dPara::xySheetsON)
					{
						ImGui::SliderFloat("Distance Z", &Eulerian3dPara::distanceZ, 0.0f, 1.0f);
					}
					if (Eulerian3dPara::yzSheetsON)
					{
						ImGui::SliderFloat("Distance X", &Eulerian3dPara::distanceX, 0.0f, 1.0f);
					}
					if (Eulerian3dPara::xzSheetsON)
					{
						ImGui::SliderFloat("Distance Y", &Eulerian3dPara::distanceY, 0.0f, 1.0f);
					}
				}
				else
				{
					ImGui::InputScalar("X-Y Sheets", ImGuiDataType_S32, &Eulerian3dPara::xySheetsNum, &intStep, NULL);
					ImGui::InputScalar("Y-Z Sheets", ImGuiDataType_S32, &Eulerian3dPara::yzSheetsNum, &intStep, NULL);
					ImGui::InputScalar("X-Z Sheets", ImGuiDataType_S32, &Eulerian3dPara::xzSheetsNum, &intStep, NULL);
				}

				ImGui::Separator();

				ImGui::Text("Solver:");
				ImGui::PushItemWidth(150);
				ImGui::SliderFloat("Delta Time", &Eulerian3dPara::dt, 0.0f, 0.1f, "%.5f");
				ImGui::SliderFloat("Source Velocity", &Eulerian3dPara::sourceVelocity, 0.0f, 5.0f);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text("Physical Parameters:");
				ImGui::SliderFloat("Air Density", &Eulerian3dPara::airDensity, 0.10f, 3.0f);
				ImGui::SliderFloat("Ambient Temperature", &Eulerian3dPara::ambientTemp, 0.0f, 50.0f);
				ImGui::SliderFloat("Boussinesq Alpha", &Eulerian3dPara::boussinesqAlpha, 100.0f, 1000.0f);
				ImGui::SliderFloat("Boussinesq Beta", &Eulerian3dPara::boussinesqBeta, 1000.0f, 5000.0f);
				ImGui::SliderFloat("Vorticity", &Eulerian3dPara::vorticityConst, 10.0f, 200.0f);
				break;

			case 4:
				// TODO(optional)
				// add other method's parameters

				break;
			}



			if (!Glb::Timer::getInstance().empty())
			{
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