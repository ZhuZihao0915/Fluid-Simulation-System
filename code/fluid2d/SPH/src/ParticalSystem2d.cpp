#include "ParticalSystem2d.h"
#include <iostream>
#include "Global.h"
#include <unordered_set>

namespace FluidSimulation {

    namespace SPH2d {
        ParticalSystem2d::ParticalSystem2d(glm::vec2 corner, glm::vec2 size) {
            lowerLeftCorner = corner;
            upperRightCorner = corner + size;
            center = corner + 0.5f * size;

            // use container's size and paticle's radius 
            // divide the container to many blocks which will be used for neighor search.
            blockRowNum = floor(2*size.y / supportRadius);
            blockColNum = floor(2*size.x / supportRadius);
            blockSize = glm::vec2(2*size.x / blockColNum, 2*size.y / blockRowNum);

            positions.clear();
            velocity.clear();
            accleration.clear();

            mStartIndex = 0;
            //mStartIndex += AddBoundary(mLowerBound, glm::vec2(size.x, mSupportRadius));
            //mStartIndex += AddBoundary(glm::vec2(mLowerBound.x, mUpperBound.y - mSupportRadius), glm::vec2(size.x, mSupportRadius));
            //mStartIndex += AddBoundary(glm::vec2(mLowerBound.x, mLowerBound.y + mSupportRadius), glm::vec2(mSupportRadius, size.y - 2.0f * mSupportRadius));
            //mStartIndex += AddBoundary(glm::vec2(mUpperBound.x - mSupportRadius, mLowerBound.y + mSupportRadius), glm::vec2(mSupportRadius, size.y - 2.0f * mSupportRadius));
        }

        int ParticalSystem2d::addFluidBlock(glm::vec2 corner, glm::vec2 size, glm::vec2 v0, float particalSpace) {
            glm::vec2 blockLowerBound = corner;
            glm::vec2 blockUpperBound = corner + size;

            // not over bound
            if (blockLowerBound.x < lowerLeftCorner.x ||
                blockLowerBound.y < lowerLeftCorner.y ||
                blockUpperBound.x > upperRightCorner.x ||
                blockUpperBound.y > upperRightCorner.y) {
                return -1;
            }

            int width = size.x / particalSpace;
            int height = size.y / particalSpace;

            std::vector<glm::vec2> pos(width * height);
            std::vector<glm::vec2> vel(width * height, v0);    // v0 is the velocity in the beginning
            std::vector<glm::vec2> acc(width * height, glm::vec2(0.0f, 0.0f));

            // init the particles' position
            int p = 0;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    pos[p] = corner + glm::vec2((j + 0.5) * particalSpace, (i + 0.5) * particalSpace);
                    if (i % 2) {
                        pos[p].x += particalRadius;
                    }
                    p++;
                }
            }

            // store the position, velocity, accleration
            positions.insert(positions.end(), pos.begin(), pos.end());
            velocity.insert(velocity.end(), vel.begin(), vel.end());
            accleration.insert(accleration.end(), acc.begin(), acc.end());
            return pos.size();
        }

        void ParticalSystem2d::searchNeighbors() {

            // build blocks
            buildBlockStructure();
            
            // build neighbor structure
            mNeighbors = std::vector<std::vector<Neighbor>>(positions.size(), std::vector<Neighbor>(0));

            for (int i = mStartIndex; i < positions.size(); i++) {
                glm::vec2 deltePos = positions[i] - lowerLeftCorner;
                uint32_t bc = floor(deltePos.x / blockSize.x);
                uint32_t br = floor(deltePos.y / blockSize.y);

                int bIdi = getBlockIdByPosition(positions[i]);

                // search the near blocks (3x3)
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        if (bc + dc < 0 || bc + dc >= blockColNum ||
                            br + dr < 0 || br + dr >= blockRowNum) {
                            continue;
                        }

                        int bIdj = bIdi + dr * blockColNum + dc;
                        // get the block
                        std::vector<int>& block = blocks[bIdj];
                        // caculate all the distance between particle i and j (in the block)
                        for (int j : block) {
                            if (i == j) {
                                continue;
                            }
                            Neighbor nInfo{};
                            nInfo.radius = positions[i] - positions[j];
                            nInfo.distance = glm::length(nInfo.radius);
                            nInfo.distance2 = nInfo.distance * nInfo.distance;
                            nInfo.index = j;
                            // if the distance lower than the support radius
                            // set this particle(j) as one of i's neighbors
                            if (nInfo.distance <= supportRadius) {
                                mNeighbors[i].push_back(nInfo);
                            }
                        }
                    }
                }
            }

            // Then we got all the particles' neighbors
        }

        size_t ParticalSystem2d::getBlockIdByPosition(glm::vec2 position)
        {
            if (position.x < lowerLeftCorner.x ||
                position.y < lowerLeftCorner.y ||
                position.x > upperRightCorner.x ||
                position.y > upperRightCorner.y) {
                return -1;
            }

            glm::vec2 deltePos = position - lowerLeftCorner;
            uint32_t c = floor(deltePos.x / blockSize.x);
            uint32_t r = floor(deltePos.y / blockSize.y);
            return r * blockColNum + c;
        }

        void ParticalSystem2d::buildBlockStructure()
        {
            blocks = new std::vector<int>[blockRowNum * blockColNum] {std::vector<int>(0)};

            // put all particles to the their blocks
            for (int i = 0; i < positions.size(); i++) {
                int bId = getBlockIdByPosition(positions[i]);
                blocks[bId].push_back(i);
            }
        }

        
        int32_t ParticalSystem2d::addBoundary(glm::vec2 corner, glm::vec2 size) {
            float space = particalRadius / 4.0f;
            int rows = floor(size.y / space);
            int cols = floor(size.x / space);

            float rowOffset = (size.y - ((float)rows - 1.0f) * space) / 2.0f;
            float colOffset = (size.x - ((float)cols - 1.0f) * space) / 2.0f;

            std::vector<glm::vec2> position(rows * cols);
            std::vector<glm::vec2> velocity(rows * cols, glm::vec2(0.0f, 0.0f));
            std::vector<glm::vec2> accleration(rows * cols, glm::vec2(0.0f, 0.0f));

            int p = 0;
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    float x = colOffset + j * space;
                    float y = rowOffset + i * space;
                    position[p] = corner + glm::vec2(x, y);
                    p++;
                }
            }

            positions.insert(positions.end(), position.begin(), position.end());
            velocity.insert(velocity.end(), velocity.begin(), velocity.end());
            accleration.insert(accleration.end(), accleration.begin(), accleration.end());
            return position.size();
        }
    }
}

