#pragma once
#ifndef __INSPECTOR_VIEW_H__
#define __INSPECTOR_VIEW_H__

#include "glad/glad.h"
#include "glfw3.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Fluid.h"
#include "Manager.h"

namespace FluidSimulation {

	class InspectorView {
	private:
		GLFWwindow* window;
		ImVec2 pos;

	public:

		int show2dID;
		int show3dID;

		InspectorView();
		InspectorView(GLFWwindow* window);
		void display2d();
		void display3d();
	};
}

#endif