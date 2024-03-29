#include "ParticleSystem2d.h"
#include <iostream>
#include "Global.h"
#include <unordered_set>

namespace FluidSimulation
{

    namespace Lagrangian2d
    {
        ParticleSystem2d::ParticleSystem2d()
        {
        }
        ParticleSystem2d::~ParticleSystem2d()
        {
        }

        void ParticleSystem2d::setContainerSize(glm::vec2 corner = glm::vec2(-1.0f, -1.0f), glm::vec2 size = glm::vec2(2.0f, 2.0f))
        {
            corner *=  Lagrangian2dPara::scale;
            size *= Lagrangian2dPara::scale;

            mLowerBound = corner - mSupportRadius + mParticleDiameter;
            mUpperBound = corner + size + mSupportRadius - mParticleDiameter;
            mContainerCenter = (mLowerBound + mUpperBound) / 2.0f;
            size = mUpperBound - mLowerBound;

            mBlockNum.x = floor(size.x / mSupportRadius);
            mBlockNum.y = floor(size.y / mSupportRadius);

            mBlockSize = glm::vec2(size.x / mBlockNum.x, size.y / mBlockNum.y);

            mBlockIdOffs.resize(9);
            int p = 0;
            for (int j = -1; j <= 1; j++)
            {
                for (int i = -1; i <= 1; i++)
                {
                    mBlockIdOffs[p] = mBlockNum.x * j + i;
                    p++;
                }
            }

            particles.clear();
        }

        int ParticleSystem2d::addFluidBlock(glm::vec2 corner, glm::vec2 size, glm::vec2 v0, float particleSpace)
        {

            corner *= Lagrangian2dPara::scale;
            size *= Lagrangian2dPara::scale;

            glm::vec2 blockLowerBound = corner;
            glm::vec2 blockUpperBound = corner + size;

            if (blockLowerBound.x < mLowerBound.x ||
                blockLowerBound.y < mLowerBound.y ||
                blockUpperBound.x > mUpperBound.x ||
                blockUpperBound.y > mUpperBound.y)
            {
                return 0;
            }

            glm::uvec2 particleNum = glm::uvec2(size.x / particleSpace, size.y / particleSpace);
            std::vector<ParticleInfo2d> tempParticles(particleNum.x * particleNum.y);

            Glb::RandomGenerator rand;
            int p = 0;
            for (int idX = 0; idX < particleNum.x; idX++)
            {
                for (int idY = 0; idY < particleNum.y; idY++)
                {
                    float x = (idX + rand.GetUniformRandom()) * particleSpace;
                    float y = (idY + rand.GetUniformRandom()) * particleSpace;

                    tempParticles[p].position = corner + glm::vec2(x, y);
                    tempParticles[p].blockId = getBlockIdByPosition(tempParticles[p].position);
                    tempParticles[p].velocity = v0;
                    p++;
                }
            }

            particles.insert(particles.end(), tempParticles.begin(), tempParticles.end());
            return particles.size();
        }

        uint32_t ParticleSystem2d::getBlockIdByPosition(glm::vec2 position)
        {
            if (position.x < mLowerBound.x ||
                position.y < mLowerBound.y ||
                position.x > mUpperBound.x ||
                position.y > mUpperBound.y)
            {
                return -1;
            }

            glm::vec2 deltePos = position - mLowerBound;
            uint32_t c = floor(deltePos.x / mBlockSize.x);
            uint32_t r = floor(deltePos.y / mBlockSize.y);
            return r * mBlockNum.x + c;
        }

        void ParticleSystem2d::updateBlockInfo()
        {
            std::sort(particles.begin(), particles.end(),
                      [=](ParticleInfo2d &first, ParticleInfo2d &second)
                      {
                          return first.blockId < second.blockId;
                      });

            mBlockExtens = std::vector<glm::uvec2>(mBlockNum.x * mBlockNum.y, glm::uvec2(0, 0));
            int curBlockId = 0;
            int left = 0;
            int right;
            for (right = 0; right < particles.size(); right++)
            {
                if (particles[right].blockId != curBlockId)
                {
                    mBlockExtens[curBlockId] = glm::uvec2(left, right);
                    left = right;
                    curBlockId = particles[right].blockId;
                }
            }
            mBlockExtens[curBlockId] = glm::uvec2(left, right);
        }

    }
}
