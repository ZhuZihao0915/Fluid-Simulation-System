#pragma once
#ifndef __PARTICAL_SYSTEM_2D_H__
#define __PARTICAL_SYSTEM_2D_H__

#include <vector>
#include <list>
#include <glm/glm.hpp>
#include "Global.h"

#include "Fluid.h"

namespace FluidSimulation {

    namespace SPH2d {

        struct Neighbor {
            int index;
            float distance;
            float distance2;
            glm::vec2 radius;
        };

        class ParticalSystem2d {
        public:

            ParticalSystem2d(glm::vec2 containerCorner, glm::vec2 containerSize);

            int32_t addFluidBlock(glm::vec2 corner, glm::vec2 size, glm::vec2 v0, float particalSpace);

            void searchNeighbors();

            size_t getBlockIdByPosition(glm::vec2 position);

            void buildBlockStructure();

        private:
            int32_t addBoundary(glm::vec2 corner, glm::vec2 size);

        public:

            int mStartIndex = 0;

            // particles' information
            std::vector<glm::vec2> positions;
            std::vector<glm::vec2> accleration;
            std::vector<glm::vec2> velocity;

            // physical attributes
            std::vector<float> mDensity;
            std::vector<float> mPressure;

            // for search
            std::vector<std::vector<Neighbor>> mNeighbors;

            // define the position and size of a container
            glm::vec2 lowerLeftCorner = glm::vec2(-1.0f, -1.0f);
            glm::vec2 upperRightCorner = glm::vec2(1.0f, 1.0f);
            glm::vec2 center = glm::vec2(0.0f, 0.0f);

            // block structure
            std::vector<int>* blocks;
            glm::vec2 blockSize;
            uint32_t blockRowNum;
            uint32_t blockColNum;

            // properties
            float particalRadius = 0.01;
            float particalDiameter = 2.0 * particalRadius;
            float supportRadius = 0.025;
            float supportRadius2 = supportRadius * supportRadius;
            float volume = 0.8 * particalDiameter * particalDiameter;
            float mass = Fluid::GetInstance().GetDensity() * volume;

        };
    }
}



#endif // !PARTICAL_SYSTEM_H

