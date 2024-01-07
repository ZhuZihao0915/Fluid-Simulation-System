#pragma once
#ifndef __SCENE_VIEW_H__
#define __SCENE_VIEW_H__

#include <iostream>

#include "glad/glad.h"
#include "glfw3.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Configure.h"
#include "Component.h"

#include "Global.h"

#include "Manager.h"
#include <thread>


namespace FluidSimulation {
	class SceneView {

	private:
		GLFWwindow* window;
		ImVec2 pos;

		Glb::Component* currentMethod;

		bool isRendering;

		// std::thread renderThread;

		GLuint texture;

	public:
		SceneView();
		SceneView(GLFWwindow* window);
		void SceneView::RenderLoop();
		void display2d();
		void display3d();
	};
}

#endif