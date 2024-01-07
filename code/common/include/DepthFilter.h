#pragma once
#ifndef __DEPTH_FILTER_H__
#define __DEPTH_FILTER_H__

#include <vector>
#include <glm/glm.hpp>
#include "ComputeShader.h"
#include "Configure.h"

namespace Glb {
    class DepthFilter {
    public:
        // 构造函数
        DepthFilter();

        // 析构函数
        ~DepthFilter();

        // 创建深度滤波器
        void Create(float_t sigma1, float_t sigma2);

        // 释放深度滤波器资源
        void Destroy();

        // 应用深度滤波器
        void Filter(GLuint& bufferA, GLuint& bufferB, glm::ivec2 imageSize);

    private:
        // 预计算操作
        void PreCalculate();

        // 构建着色器
        void BuildShader();

        // 上传缓冲区数据到GPU
        void Uploadbuffers();

        // 计算深度滤波器权重
        float_t CalculateWeight(float_t d1, float_t d2);

        // 生成滤波核的索引
        std::vector<glm::ivec2> GenerateIndexes(int32_t halfkernelSize);

    private:
        // 存储深度滤波器的权重
        std::vector<float_t> mWeightBuffer;

        // 图像大小
        glm::ivec2 mBufferSize;

        // 深度滤波器参数
        float_t mSigma1;
        float_t mSigma2;
        int32_t mHalfKernelSize;

        // GPU上执行深度滤波的着色器
        Glb::ComputeShader* mBlurZ = nullptr;

        // GPU纹理对象
        GLuint mTexDepthFilter = 0;

        // GPU缓冲区对象，用于5x5和9x9滤波核的索引
        GLuint mBufferKernelIndexs5x5 = 0;
        GLuint mBufferKernelIndexs9x9 = 0;
    };

}


#endif
