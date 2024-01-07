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
			float gravity = SPH2d::Fluid::GetInstance().GetGravity();
			ImGui::SliderFloat("Gravity", &gravity, -20.0f, 20.0f);
			SPH2d::Fluid::GetInstance().SetGravity(gravity);

			float density = SPH2d::Fluid::GetInstance().GetDensity();
			ImGui::SliderFloat("Normal Density", &density, 500.0f, 1500.0f);
			SPH2d::Fluid::GetInstance().SetDensity(density);

			float stiffness = SPH2d::Fluid::GetInstance().GetStiffness();
			ImGui::SliderFloat("Stiffness", &stiffness, 10.0f, 100.0f);
			SPH2d::Fluid::GetInstance().SetStiffness(stiffness);

			float exponent = SPH2d::Fluid::GetInstance().GetExponent();
			ImGui::SliderFloat("Exponent", &exponent, 5.0f, 10.0f);
			SPH2d::Fluid::GetInstance().SetExponent(exponent);

			float viscosity = SPH2d::Fluid::GetInstance().GetViscosity();
			ImGui::SliderFloat("Viscosity", &viscosity, 0.01f, 0.04f);
			SPH2d::Fluid::GetInstance().SetViscosity(viscosity);

		}
		if (show2dID == 2) {
			ImGui::Text("Container Parameters:");
			float corner_x = SPH2d::Container::GetInstance().GetCornerX();
			ImGui::SliderFloat("corner.x", &corner_x, -1.0f, 1.0f);
			SPH2d::Container::GetInstance().SetCornerX(corner_x);

			float corner_y = SPH2d::Container::GetInstance().GetCornerY();
			ImGui::SliderFloat("corner.y", &corner_y, -1.0f, 1.0f);
			SPH2d::Container::GetInstance().SetCornerY(corner_y);

			float size_x = SPH2d::Container::GetInstance().GetSizeX();
			ImGui::SliderFloat("size.x", &size_x, 0.0f, 2.0f);
			SPH2d::Container::GetInstance().SetSizeX(size_x);

			float size_y = SPH2d::Container::GetInstance().GetSizeY();
			ImGui::SliderFloat("size.y", &size_y, 0.0f, 2.0f);
			SPH2d::Container::GetInstance().SetSizeY(size_y);

			ImGui::Text("*You have to reset to\n apply the new\n container parameters.");
		}
		else if (show2dID == 3) {
			ImGui::Text("Iteration Parameters:");
			/*float dt = Iteration::GetInstance().GetDt();
			float dt_times = 1;
			ImGui::SliderFloat("dt times", &dt_times, 0.1f, 10.0f);
			Iteration::GetInstance().SetDt(dt * dt_times);*/

			float substep = SPH2d::Iteration::GetInstance().GetSubstep();
			ImGui::SliderFloat("substep", &substep, 1, 30);
			SPH2d::Iteration::GetInstance().SetSubstep(substep);
		}

		ImGui::End();
	}

	void InspectorView::display3d() {

	}
}