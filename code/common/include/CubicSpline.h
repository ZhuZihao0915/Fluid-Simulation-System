#pragma once
#ifndef W_CUBE_SPLINE_H
#define W_CUBE_SPLINE_H

#include <glm/glm.hpp>
#include <vector>

namespace Glb {
    class CubicSpline2d {
    public:
        CubicSpline2d() = delete;
        explicit CubicSpline2d(float h);
        ~CubicSpline2d();

        float Value(float distance);

        glm::vec2 Grad(glm::vec2 radius);
        
    private:
        float CalculateValue(float distance);

        glm::vec2 CalculateGrad(glm::vec2 radius);

    private:
        float mH;
        float mH2;
        float mSigma;
        glm::uvec2 mBufferSize;
        std::vector<std::vector<glm::vec2>> mGradBuffer;
        std::vector<float> mValueBuffer;
    };

    class CubicSpline3d {
    public:
        CubicSpline3d() = delete;
        explicit CubicSpline3d(float h);
        ~CubicSpline3d();
        
        float_t* GetData();
        uint32_t GetBufferSize();
        glm::vec2 GetValueAndGrad(float f);

    private:
        float CalculateValue(float distance);

        float CalculateGradFactor(float distance);

    private:
        float mH;
        float mH2;
        float mH3;
        float mSigma;
        uint32_t mBufferSize;
        std::vector<glm::vec2> mValueAndGradFactorBuffer;
    };

}

#endif // !W_CUBE_SPLINE_H
