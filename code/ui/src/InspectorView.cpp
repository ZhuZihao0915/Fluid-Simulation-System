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

		if (Manager::GetInstance().GetSceneView()->currentMethod==NULL) {
			ImGui::Text("No Simulation Method!\nPlease select a simulation method.");
		}
		else{
			switch (Manager::GetInstance().GetSceneView()->currentMethod->id)
			{
			// sph 2d
			case 0:
				switch (showID)
				{

				}
			// mac 3d
			case 3:
				switch (showID)
				{
				// camera
				case 0:
					float floatStep = 0.1;
					ImGui::InputFloat3("Position", &Manager::GetInstance().GetSceneView()->currentMethod->camera->mPosition.x);
					ImGui::InputScalar("Fov", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->fovyDeg, &floatStep, NULL);
					ImGui::InputScalar("Aspect", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->aspect, &floatStep, NULL);
					ImGui::InputScalar("Near", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->nearPlane, &floatStep, NULL);
					ImGui::InputScalar("Far", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->farPlane, &floatStep, NULL);
					ImGui::InputScalar("Yaw", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->mYaw, &floatStep, NULL);
					ImGui::InputScalar("Pitch", ImGuiDataType_Float, &Manager::GetInstance().GetSceneView()->currentMethod->camera->mPitch, &floatStep, NULL);
					Manager::GetInstance().GetSceneView()->currentMethod->camera->UpdateView();
				}
			}
		}

		ImGui::End();
	}
}