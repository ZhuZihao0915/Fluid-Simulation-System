#pragma once
#ifndef __PARTICAL_SYSTEM_2D_H__
#define __PARTICAL_SYSTEM_2D_H__

#include <vector>
#include <list>
#include <glm/glm.hpp>
#include "Global.h"

#include "Configure.h"

namespace FluidSimulation
{

    namespace Lagrangian2d
    {

        struct ParticalInfo2d
        {
            alignas(8) glm::vec2 position;
            alignas(8) glm::vec2 velocity;
            alignas(8) glm::vec2 accleration;
            alignas(4) float density;
            alignas(4) float pressure;
            alignas(4) float pressDivDens2;
            ;
            alignas(4) uint32_t blockId;
        };

        class ParticalSystem2d
        {
        public:
            ParticalSystem2d();
            ~ParticalSystem2d();

            void setContainerSize(glm::vec2 containerCorner, glm::vec2 containerSize);
            int32_t addFluidBlock(glm::vec2 corner, glm::vec2 size, glm::vec2 v0, float particalSpace);
            uint32_t getBlockIdByPosition(glm::vec2 position);
            void updateBlockInfo();

        public:
            float mSupportRadius = Lagrangian2dPara::supportRadius;
            float mSupportRadius2 = mSupportRadius * mSupportRadius;
            float mParticalRadius = Lagrangian2dPara::particalRadius;
            float mParticalDiameter = Lagrangian2dPara::particalDiameter;
            float mVolume = mParticalDiameter * mParticalDiameter;

            std::vector<ParticalInfo2d> mParticalInfos;
            int mMaxNeighbors = 128;

            glm::vec2 mLowerBound = glm::vec2(FLT_MAX);
            glm::vec2 mUpperBound = glm::vec2(-FLT_MAX);
            glm::vec2 mContainerCenter = glm::vec2(0.0f);
            glm::uvec2 mBlockNum = glm::uvec2(0);
            glm::vec2 mBlockSize = glm::vec2(0.0f);

            std::vector<glm::uvec2> mBlockExtens;
            std::vector<int32_t> mBlockIdOffs;
        };
    }
}

#endif // !PARTICAL_SYSTEM_H
