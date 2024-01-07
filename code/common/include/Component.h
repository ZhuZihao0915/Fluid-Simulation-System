#pragma once
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <glfw3.h>

// 流体模拟算法组件
// 用于组织粒子/网格系统、渲染器、求解器

namespace Glb {
	class Component {
	public:
		char* description;
		Component() {
			this->description = NULL;
		}
		Component(char* description) {
			this->description = description;
		}

		virtual void shutDown() = 0;
		virtual void init() = 0;
		virtual void simulate() = 0;
		virtual GLuint getRenderedTexture() = 0;
	};
}

#endif