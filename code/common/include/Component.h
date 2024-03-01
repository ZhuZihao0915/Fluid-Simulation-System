#pragma once
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <glfw3.h>
#include "Camera.h"

// ������֯����/����ϵͳ����Ⱦ���������

namespace Glb {
	class Component {
	public:
		int id;
		bool is3D;
		char* description;

		Component() {
			this->description = NULL;
		}
		Component(char* description) {
			this->description = description;
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