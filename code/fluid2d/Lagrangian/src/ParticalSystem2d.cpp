#include "ParticalSystem2d.h"
#include <iostream>
#include "Global.h"
#include <unordered_set>

namespace FluidSimulation {

    namespace Lagrangian2d {
        ParticalSystem2d::ParticalSystem2d(){
            
        }
        ParticalSystem2d::~ParticalSystem2d() {

        }

        void ParticalSystem2d::setContainerSize(glm::vec2 corner, glm::vec2 size) {
            mLowerBound = corner - mSupportRadius + mParticalDiameter;
            mUpperBound = corner + size + mSupportRadius - mParticalDiameter;
            mContainerCenter = (mLowerBound + mUpperBound) / 2.0f;
            size = mUpperBound - mLowerBound;

            // 两个方向的block数量
            mBlockNum.x = floor(size.x / mSupportRadius);
            mBlockNum.y = floor(size.y / mSupportRadius);
            
            // 一个block的大小
            mBlockSize = glm::vec2(size.x / mBlockNum.x, size.y / mBlockNum.y);

            mBlockIdOffs.resize(9);
            int p = 0;
            for (int j = -1; j <= 1; j++) {
                for (int i = -1; i <= 1; i++) {
                    mBlockIdOffs[p] = mBlockNum.x * j + i;
                    p++;
                }
            }

            mParticalInfos.clear();
        }

        int ParticalSystem2d::addFluidBlock(glm::vec2 corner, glm::vec2 size, glm::vec2 v0, float particalSpace) {
            glm::vec2 blockLowerBound = corner;
            glm::vec2 blockUpperBound = corner + size;

            if (blockLowerBound.x < mLowerBound.x ||
                blockLowerBound.y < mLowerBound.y ||
                blockUpperBound.x > mUpperBound.x ||
                blockUpperBound.y > mUpperBound.y) {
                return 0;
            }

            glm::uvec2 particalNum = glm::uvec2(size.x / particalSpace, size.y / particalSpace);
            std::vector<ParticalInfo2d> particals(particalNum.x * particalNum.y);

            Glb::RandomGenerator rand;
            int p = 0;
            for (int idX = 0; idX < particalNum.x; idX++) {
                for (int idY = 0; idY < particalNum.y; idY++) {
                    float x = (idX + rand.GetUniformRandom()) * particalSpace;
                    float y = (idY + rand.GetUniformRandom()) * particalSpace;

                    particals[p].position = corner + glm::vec2(x, y);
                    particals[p].blockId = getBlockIdByPosition(particals[p].position);
                    particals[p].velocity = v0;
                    p++;
                }
            }

            mParticalInfos.insert(mParticalInfos.end(), particals.begin(), particals.end());
            return particals.size();
        }
        
        uint32_t ParticalSystem2d::getBlockIdByPosition(glm::vec2 position)
        {
            if (position.x < mLowerBound.x ||
                position.y < mLowerBound.y ||
                position.x > mUpperBound.x ||
                position.y > mUpperBound.y) {
                return -1;
            }

            glm::vec2 deltePos = position - mLowerBound;
            uint32_t c = floor(deltePos.x / mBlockSize.x);
            uint32_t r = floor(deltePos.y / mBlockSize.y);
            return r * mBlockNum.x + c;
        }

        void ParticalSystem2d::updateBlockInfo() {

            // 按blockId的顺序，对粒子进行排序
            // 排序前要求各个粒子的blockID已经被正确更新
            // 在addFluidBlock中，对粒子的blockID初始化
            // 之后的模拟过程中，Slover::calculateBlockId()用来更新blockID
            std::sort(mParticalInfos.begin(), mParticalInfos.end(),
                [=](ParticalInfo2d& first, ParticalInfo2d& second) {
                    return first.blockId < second.blockId;
                }
            );

            // 更新每个block在排序后的粒子数组中的起始和结束索引
            mBlockExtens = std::vector<glm::uvec2>(mBlockNum.x * mBlockNum.y, glm::uvec2(0, 0));
            int curBlockId = 0;
            int left = 0;
            int right;
            for (right = 0; right < mParticalInfos.size(); right++) {
                if (mParticalInfos[right].blockId != curBlockId) {
                    mBlockExtens[curBlockId] = glm::uvec2(left, right);        // 左闭右开
                    left = right;
                    curBlockId = mParticalInfos[right].blockId;
                }
            }
            mBlockExtens[curBlockId] = glm::uvec2(left, right);
        }

    }
}

