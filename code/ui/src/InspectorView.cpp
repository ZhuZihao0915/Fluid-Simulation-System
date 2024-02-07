#include "InspectorView.h"


namespace FluidSimulation {
	InspectorView::InspectorView() {

	}

	InspectorView::InspectorView(GLFWwindow* window) {
		this->window = window;
		show2dID = false;
	}

	void InspectorView::display2d() {
		ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoCollapse);

		if (show2dID == 1) {
			ImGui::Text("Fluid Parameters:");

			ImGui::SliderFloat("Gravity", &SPH2dPara::gravity, -20.0f, 20.0f);

			ImGui::SliderFloat("Normal Density", &SPH2dPara::density, 500.0f, 1500.0f);

			ImGui::SliderFloat("Stiffness", &SPH2dPara::stiffness, 10.0f, 100.0f);

			ImGui::SliderFloat("Exponent", &SPH2dPara::exponent, 5.0f, 10.0f);

			ImGui::SliderFloat("Viscosity", &SPH2dPara::viscosity, 0.01f, 0.04f);

		}
		if (show2dID == 2) {


			ImGui::Text("*You have to reset to\n apply the new\n container parameters.");
		}
		else if (show2dID == 3) {
			ImGui::Text("Iteration Parameters:");
			ImGui::SliderFloat("dt times", &SPH2dPara::dt, 0.1f, 10.0f);

			ImGui::SliderInt("substep", &SPH2dPara::substep, 1, 15);
		}

		ImGui::End();
	}

	void InspectorView::display3d() {

	}
}