#include "ParticleSystem3d.h"
#include <iostream>
#include <algorithm>
#include <Global.h>

namespace FluidSimulation
{
    namespace Lagrangian3d
    {
        ParticleSystem3d::ParticleSystem3d()
        {
        }

        ParticleSystem3d::~ParticleSystem3d()
        {
        }

        void ParticleSystem3d::setContainerSize(glm::vec3 corner, glm::vec3 size)
        {
            mLowerBound = corner - mSupportRadius + mParticleDiameter;
            mUpperBound = corner + size + mSupportRadius - mParticleDiameter;
            mContainerCenter = (mLowerBound + mUpperBound) / 2.0f;
            size = mUpperBound - mLowerBound;

            // 三个方向的block数量
            mBlockNum.x = floor(size.x / mSupportRadius);
            mBlockNum.y = floor(size.y / mSupportRadius);
            mBlockNum.z = floor(size.z / mSupportRadius);

            // 一个block的大小
            mBlockSize = glm::vec3(size.x / mBlockNum.x, size.y / mBlockNum.y, size.z / mBlockNum.z);

            mBlockIdOffs.resize(27);
            int p = 0;
            for (int k = -1; k <= 1; k++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    for (int i = -1; i <= 1; i++)
                    {
                        mBlockIdOffs[p] = mBlockNum.x * mBlockNum.y * k + mBlockNum.x * j + i;
                        p++;
                    }
                }
            }

            mParticleInfos.clear();
        }

        int32_t ParticleSystem3d::addFluidBlock(glm::vec3 corner, glm::vec3 size, glm::vec3 v0, float particleSpace)
        {
            glm::vec3 blockLowerBound = corner;
            glm::vec3 blockUpperBound = corner + size;

            if (blockLowerBound.x < mLowerBound.x ||
                blockLowerBound.y < mLowerBound.y ||
                blockLowerBound.z < mLowerBound.z ||
                blockUpperBound.x > mUpperBound.x ||
                blockUpperBound.y > mUpperBound.y ||
                blockUpperBound.z > mUpperBound.z)
            {
                return 0;
            }

            glm::uvec3 particleNum = glm::uvec3(size.x / particleSpace, size.y / particleSpace, size.z / particleSpace);
            std::vector<ParticleInfo3d> particles(particleNum.x * particleNum.y * particleNum.z);

            Glb::RandomGenerator rand;
            int p = 0;
            for (int idX = 0; idX < particleNum.x; idX++)
            {
                for (int idY = 0; idY < particleNum.y; idY++)
                {
                    for (int idZ = 0; idZ < particleNum.z; idZ++)
                    {
                        float x = (idX + rand.GetUniformRandom()) * particleSpace;
                        float y = (idY + rand.GetUniformRandom()) * particleSpace;
                        float z = (idZ + rand.GetUniformRandom()) * particleSpace;
                        particles[p].position = corner + glm::vec3(x, y, z);
                        particles[p].blockId = getBlockIdByPosition(particles[p].position);
                        particles[p].velocity = v0;
                        p++;
                    }
                }
            }

            mParticleInfos.insert(mParticleInfos.end(), particles.begin(), particles.end());
            return particles.size();
        }

        uint32_t ParticleSystem3d::getBlockIdByPosition(glm::vec3 position)
        {
            if (position.x < mLowerBound.x ||
                position.y < mLowerBound.y ||
                position.z < mLowerBound.z ||
                position.x > mUpperBound.x ||
                position.y > mUpperBound.y ||
                position.z > mUpperBound.z)
            {
                return -1;
            }

            glm::vec3 deltePos = position - mLowerBound;
            uint32_t c = floor(deltePos.x / mBlockSize.x);
            uint32_t r = floor(deltePos.y / mBlockSize.y);
            uint32_t h = floor(deltePos.z / mBlockSize.z);
            return h * mBlockNum.x * mBlockNum.y + r * mBlockNum.x + c;
        }

        void ParticleSystem3d::updateBlockInfo()
        {

            // 按blockId的顺序，对粒子进行排序
            // 排序前要求各个粒子的blockID已经被正确更新
            // 在addFluidBlock中，对粒子的blockID初始化
            // 之后的模拟过程中，Slover::calculateBlockId()用来更新blockID
            std::sort(mParticleInfos.begin(), mParticleInfos.end(),
                      [=](ParticleInfo3d &first, ParticleInfo3d &second)
                      {
                          return first.blockId < second.blockId;
                      });

            // 更新每个block在排序后的粒子数组中的起始和结束索引
            mBlockExtens = std::vector<glm::uvec2>(mBlockNum.x * mBlockNum.y * mBlockNum.z, glm::uvec2(0, 0));
            int curBlockId = 0;
            int left = 0;
            int right;
            for (right = 0; right < mParticleInfos.size(); right++)
            {
                if (mParticleInfos[right].blockId != curBlockId)
                {
                    mBlockExtens[curBlockId] = glm::uvec2(left, right); // 左闭右开
                    left = right;
                    curBlockId = mParticleInfos[right].blockId;
                }
            }
            mBlockExtens[curBlockId] = glm::uvec2(left, right);
        }

    }
}
