#include "ProjectView.h"

namespace FluidSimulation {
	ProjectView::ProjectView() {

	}

	ProjectView::ProjectView(GLFWwindow* window) {
		this->window = window;

	}

	void ProjectView::display() {
		ImGui::Begin("Project", NULL, ImGuiWindowFlags_NoCollapse);

		for each (char* log in Manager::GetInstance().GetLog())
		{
			ImGui::Text(log);
		}

		ImGui::End();
	}
}