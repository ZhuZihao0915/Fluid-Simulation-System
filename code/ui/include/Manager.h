#pragma once
#ifndef __MANAGER_H__
#define __MANAGER_H__

#include "glad/glad.h"
#include "glfw3.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "HierarchyView.h"
#include "SceneView.h"
#include "InspectorView.h"
#include "ProjectView.h"

#include "Sph2dComponent.h"
#include "Mac2dComponent.h"
#include "Sph3dComponent.h"
#include "Mac3dComponent.h"

#include <vector>

namespace FluidSimulation
{
	class HierarchyView;
	class SceneView;
	class InspectorView;
	class ProjectView;

	class Manager {
	public:
		static Manager& GetInstance() {
			static Manager instance;
			return instance;
		}

		void init(GLFWwindow* window);
		void displayViews();
		void displayToolBar();
		void pushMessage(char* message);

		HierarchyView* GetHierachyView() const { return hierarchyView; };
		SceneView* GetSceneView() const { return sceneView; };
		InspectorView* GetInspectorView() const { return inspectorView; };
		ProjectView* GetProjectView() const { return projectView; };
		GLFWwindow* GetWindow() const { return window; };
		std::vector<char*> GetLog() const { return log; };

	private:

		Manager() {
			window = NULL;
			hierarchyView = NULL;
			sceneView = NULL;
			inspectorView = NULL;
			projectView = NULL;
		};

		Manager(const Manager&) = delete;
		Manager& operator=(const Manager&) = delete;
		
		GLFWwindow* window;
		HierarchyView* hierarchyView;
		SceneView* sceneView;
		InspectorView* inspectorView;
		ProjectView* projectView;

		std::vector<char*> log;
		
	};
}

#endif