#pragma once
#ifndef __HIERARCHY_VIEW_H__
#define __HIERARCHY_VIEW_H__

#include "glad/glad.h"
#include "glfw3.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Configure.h"
#include "Manager.h"

namespace FluidSimulation {

	class HierarchyView {

	private:
		GLFWwindow* window;
		ImVec2 pos;

		bool selected[10] = { false };

	public:
		HierarchyView();
		HierarchyView(GLFWwindow* window);
		void display();
	};
}

#endif