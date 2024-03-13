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
        // ��ӷ���
        int id = 0;
        methodComponents.push_back(new Lagrangian2d::Lagrangian2dComponent("Lagrangian 2d", id++));
        methodComponents.push_back(new Eulerian2d::Eulerian2dComponent("Eulerian 2d", id++));
        methodComponents.push_back(new Lagrangian3d::Lagrangian3dComponent("Lagrangian 3d", id++));
        methodComponents.push_back(new Eulerian3d::Eulerian3dComponent("Eulerian 3d", id++));
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
        // hierarchyView->display();
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