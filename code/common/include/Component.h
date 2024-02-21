#pragma once
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <glfw3.h>
#include "Camera.h"

// ����ģ���㷨���
// ������֯����/����ϵͳ����Ⱦ���������

namespace Glb {
	class Component {
	public:
		int id;
		bool is3D;
		char* description;
		Camera* camera;

		Component() {
			this->description = NULL;
			this->camera = NULL;
		}
		Component(char* description) {
			this->description = description;
			this->camera = NULL;
		}

		virtual void cameraRotate(float x, float y) { if (!is3D)return; };
		virtual void cameraMove(float x, float y) { if (!is3D)return; };
		virtual void cameraScale(float w) { if (!is3D)return; };
		virtual void shutDown() = 0;
		virtual void init() = 0;
		virtual void simulate() = 0;
		virtual GLuint getRenderedTexture() = 0;
	};
}

#endif