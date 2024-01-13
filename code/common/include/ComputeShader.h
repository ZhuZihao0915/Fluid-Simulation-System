#pragma once
#ifndef __COMPUTE_SHADER_H__
#define __COMPUTE_SHADER_H__

#include <glad/glad.h>
#include <string>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

namespace Glb {

    class ComputeShader {
    public:
        ComputeShader() = delete;
        ComputeShader(std::string name);
        ~ComputeShader();

        int32_t buildFromFile(std::string& compPath);
        int32_t buildFromFiles(std::vector<std::string>& compPaths);
        void use();
        void unUse();
        GLuint getId();

        void setBool(const std::string& name, bool value);
        void setInt(const std::string& name, int value);
        void setUInt(const std::string& name, uint32_t value);
        void setFloat(const std::string& name, float value);
        void setVec2(const std::string& name, const glm::vec2& value);
        void setVec2(const std::string& name, float x, float y);
        void setVec3(const std::string& name, const glm::vec3& value);
        void setVec3(const std::string& name, float x, float y, float z);
        void setUVec3(const std::string& name, const glm::uvec3& value);
        void setVec4(const std::string& name, const glm::vec4& value);
        void setVec4(const std::string& name, float x, float y, float z, float w);
        void setMat2(const std::string& name, const glm::mat2& mat);
        void setMat3(const std::string& name, const glm::mat3& mat);
        void setMat4(const std::string& name, const glm::mat4& mat);

    private:
        std::string mName;
        GLuint mId = 0;

    };
}

#endif // !COMPUTE_SHADER_H
