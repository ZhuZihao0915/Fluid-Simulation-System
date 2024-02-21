#include "Manager.h"

namespace FluidSimulation
{

    void Manager::init(GLFWwindow* window) {
        this->window = window;
        hierarchyView = new HierarchyView(window);
        inspectorView = new InspectorView(window);
        projectView =  new ProjectView(window);
        sceneView = new SceneView(window);
    

        // TODO
        // 添加模拟方法
        int id = 0;
        methodComponents.push_back(new SPH2d::Sph2dComponent("sph 2d", id++));
        methodComponents.push_back(new MAC2d::Mac2dComponent("mac 2d", id++));
        methodComponents.push_back(new SPH3d::Sph3dComponent("sph 3d", id++));
        methodComponents.push_back(new MAC3d::Mac3dComponent("mac 3d", id++));
    }

	void Manager::displayViews() {

        // set DockSpace flags
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGui::DockSpaceOverViewport(nullptr, dockspace_flags);

        // set theme color
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

        // display all views
        hierarchyView->display();
        sceneView->display();
        inspectorView->display();
        projectView->display();
	}

    void Manager::pushMessage(char* message) {
        log.push_back(message);
    }

    void Manager::displayToolBar() {

    }
}