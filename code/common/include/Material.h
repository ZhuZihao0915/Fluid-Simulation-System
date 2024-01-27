﻿#ifndef MATERIAL_H
#define MATERIAL_H

#include <glad/glad.h>
#include "glfw3.h"
#include <string>

namespace Glb {
    class Material {
    public:
        Material();
        ~Material();

        void Create();
        void Destroy();
        void LoadTextures(std::string& albedoPath, std::string& roughnessPath);

    private:
        void LoadTexture(GLuint texId, std::string& path);

    public:
        GLuint mTexAlbedo = 0;
        GLuint mTexRoughness = 0;
    };

}

#endif // !MATERIAL_H

