#pragma once
#ifndef __PARTICAL_SYSTEM_3D_H__
#define __PARTICAL_SYSTEM_3D_H__

#include <glm/glm.hpp>
#include <vector>
#include "Configure.h"
#include "WCubicSpline.h"

namespace FluidSimulation {

    namespace SPH3d {

        struct ParticalInfo3d
        {
            alignas(16) glm::vec3 position;
            alignas(16) glm::vec3 velocity;
            alignas(16) glm::vec3 accleration;
            alignas(4) float_t density;
            alignas(4) float_t pressure;
            alignas(4) float_t pressDivDens2;;
            alignas(4) uint32_t blockId;
        };

        struct NeighborInfo {
            alignas(16) glm::vec3 radius;
            alignas(4) float_t distance;
            alignas(4) int32_t neighborId;
        };

        class ParticalSystem3d {
        public:
            ParticalSystem3d();
            ~ParticalSystem3d();

            void setContainerSize(glm::vec3 corner, glm::vec3 size);
            int32_t addFluidBlock(glm::vec3 corner, glm::vec3 size, glm::vec3 v0, float particalSpace);
            uint32_t getBlockIdByPosition(glm::vec3 position);
            void updateBlockInfo();
        public:
            // 粒子参数
            float mSupportRadius = SPH3D::supportRadius;    // 支撑半径
            float mSupportRadius2 = mSupportRadius * mSupportRadius;
            float mParticalRadius = SPH3D::particalRadius;   // 粒子半径
            float mParticalDiameter = SPH3D::particalDiameter;
            float mVolume = std::pow(mParticalDiameter, 3);    // 体积
            float mMass = SPH3D::density0 * mVolume;  // 质量
            float mViscosity = SPH3D::viscosity;            // 粘度系数
            float mExponent = SPH3D::exponent;              // 压力指数
            int mStiffness = SPH3D::stiffness;            // 刚度
            std::vector<ParticalInfo3d> mParticalInfos;
            int mMaxNeighbors = 512;

            // 容器参数
            glm::vec3 mLowerBound = glm::vec3(FLT_MAX);
            glm::vec3 mUpperBound = glm::vec3(-FLT_MAX);
            glm::vec3 mContainerCenter = glm::vec3(0.0f);
            glm::uvec3 mBlockNum = glm::uvec3(0);    // XYZ轴有几个block
            glm::vec3 mBlockSize = glm::vec3(0.0f);
            std::vector<glm::uvec2> mBlockExtens;   // 记载着每个block含有那个索引区间的粒子（索引为mParticalInfos的索引）
            std::vector<int32_t> mBlockIdOffs;

        };

    }
}

#endif // !PARTICAL_SYSTEM_3D_H