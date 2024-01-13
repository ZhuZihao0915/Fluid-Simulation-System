#pragma once
#ifndef __SKY_BOX_H__
#define __SKY_BOX_H__

#include <vector>
#include <string>
#include "glad/glad.h"
#include <glfw3.h>
#include "Shader.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION

namespace Glb {
    class SkyBox {
    public:
        SkyBox();
        ~SkyBox();

        void Create();
        void Destroy();

        int32_t LoadImages(std::vector<std::string> paths);

        void BuildShader();

        void Draw(GLFWwindow* window, GLuint nullVao, glm::mat4 view, glm::mat4 proj);

        GLuint getId();

    private:
        GLuint mId = 0;
        Glb::Shader* mShader = nullptr;

    };
}


#endif
