#include "ComputeShader.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Glb {
    ComputeShader::ComputeShader(std::string name) {
        mName = name;
    }

    ComputeShader::~ComputeShader() {
    
    }

    int32_t ComputeShader::buildFromFile(std::string& compPath) {
        std::string shaderCode;
        std::ifstream shaderFile;
        std::stringstream shaderStream;
        // 读取 compute shader
        shaderFile.open(compPath);
        if (!shaderFile) {
            std::cout << "ERROR: Compute shader file open failed name:" << mName <<  std::endl;
            return -1;
        }
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
        const char* shaderCodeBuffer = shaderCode.c_str();

        // create shader
        GLuint computeShader;
        computeShader = glCreateShader(GL_COMPUTE_SHADER);  // 使用GL_COMPUTE_SHADER
        glShaderSource(computeShader, 1, &shaderCodeBuffer, NULL);
        // compile and checkerrors
        glCompileShader(computeShader);
        int success;
        char infoLog[1024];
        glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(computeShader, 1024, NULL, infoLog);
            std::cout << "ERROR::CONPUTE_SHADER_COMPILATION_ERROR name:" << mName << "\n" << infoLog << std::endl;
            return -1;
        }

        // generate and link shader program
        mId = glCreateProgram();
        glAttachShader(mId, computeShader);
        
        glLinkProgram(mId);
        // check link errors
        glGetProgramiv(mId, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(mId, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR mName:" << mName << "\n" << infoLog << std::endl;
            return -1;
        }
        // delete the shaders
        glDeleteShader(computeShader);
        std::cout << "compute shader success mName:" << mName << std::endl;

        return 0;
    }

    int32_t ComputeShader::buildFromFiles(std::vector<std::string>& compPaths) {
        std::vector<GLuint> computeShaders(compPaths.size());
        for (int i = 0; i < compPaths.size(); i++) {
            std::string shaderCode;
            std::ifstream shaderFile;
            std::stringstream shaderStream;
            // 读取 compute shader
            shaderFile.open(compPaths[i]);
            if (!shaderFile) {
                std::cout << "error: Compute shader file " << i << " open failed mName:" << mName << std::endl;
                return -1;
            }
            shaderStream << shaderFile.rdbuf();
            shaderFile.close();
            shaderCode = shaderStream.str();
            const char* shaderCodeBuffer = shaderCode.c_str();

            // create shader
            computeShaders[i] = glCreateShader(GL_COMPUTE_SHADER);
            glShaderSource(computeShaders[i], 1, &shaderCodeBuffer, NULL);
            // compile and checkerrors
            glCompileShader(computeShaders[i]);
            int success;
            char infoLog[1024];
            glGetShaderiv(computeShaders[i], GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(computeShaders[i], 1024, NULL, infoLog);
                std::cout << "error: compute shader" << i << "compile error mName:" << mName << "\n" << infoLog << std::endl;
                return -1;
            }
        }


        // generate and link shader program
        mId = glCreateProgram();
        for (int i = 0; i < computeShaders.size(); i++) {
            glAttachShader(mId, computeShaders[i]);
        }
        glLinkProgram(mId);
        // check link errors
        int success;
        char infoLog[1024];
        glGetProgramiv(mId, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(mId, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR mName:" << mName << "\n" << infoLog << std::endl;
            return -1;
        }
        // delete the shaders
        for (int i = 0; i < computeShaders.size(); i++) {
            glDeleteShader(computeShaders[i]);
        }
        std::cout << "compute shader build files success mName:" << mName << std::endl;
    }

    void ComputeShader::use() {
        glUseProgram(mId);
    }

    void ComputeShader::unUse() {
        glUseProgram(0);
    }

    GLuint ComputeShader::getId() {
        return mId;
    }

    void ComputeShader::setBool(const std::string& name, bool value)
    {
        glUniform1i(glGetUniformLocation(mId, name.c_str()), (int)value);
    }

    void ComputeShader::setInt(const std::string& name, int value)
    {
        glUniform1i(glGetUniformLocation(mId, name.c_str()), value);
    }

    void ComputeShader::setUInt(const std::string& name, uint32_t value) {
        glUniform1ui(glGetUniformLocation(mId, name.c_str()), value);
    }

    void ComputeShader::setFloat(const std::string& name, float value)
    {
        glUniform1f(glGetUniformLocation(mId, name.c_str()), value);
    }

    void ComputeShader::setVec2(const std::string& name, const glm::vec2& value)
    {
        glUniform2fv(glGetUniformLocation(mId, name.c_str()), 1, &value[0]);
    }
    void ComputeShader::setVec2(const std::string& name, float x, float y)
    {
        glUniform2f(glGetUniformLocation(mId, name.c_str()), x, y);
    }

    void ComputeShader::setVec3(const std::string& name, const glm::vec3& value)
    {
        glUniform3fv(glGetUniformLocation(mId, name.c_str()), 1, &value[0]);
    }

    void ComputeShader::setVec3(const std::string& name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(mId, name.c_str()), x, y, z);
    }

    void ComputeShader::setUVec3(const std::string& name, const glm::uvec3& value) {
        glUniform3uiv(glGetUniformLocation(mId, name.c_str()), 1, &value[0]);
    }

    void ComputeShader::setVec4(const std::string& name, const glm::vec4& value)
    {
        glUniform4fv(glGetUniformLocation(mId, name.c_str()), 1, &value[0]);
    }
    void ComputeShader::setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(mId, name.c_str()), x, y, z, w);
    }

    void ComputeShader::setMat2(const std::string& name, const glm::mat2& mat)
    {
        glUniformMatrix2fv(glGetUniformLocation(mId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ComputeShader::setMat3(const std::string& name, const glm::mat3& mat)
    {
        glUniformMatrix3fv(glGetUniformLocation(mId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ComputeShader::setMat4(const std::string& name, const glm::mat4& mat)
    {
        glUniformMatrix4fv(glGetUniformLocation(mId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
}