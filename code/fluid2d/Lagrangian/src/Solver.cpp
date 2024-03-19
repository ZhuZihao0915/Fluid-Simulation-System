#include "Lagrangian/include/Solver.h"
#include "Global.h"
#include <iostream>
#include <algorithm>

namespace FluidSimulation
{

    namespace Lagrangian2d
    {
        Solver::Solver(ParticleSystem2d &ps) : mPs(ps), mW(ps.mSupportRadius)
        {
        }

        Solver::~Solver()
        {
        }

        void Solver::solve()
        {
            // TODO
            // Solves the fluid simulation by performing some steps, which may include:
            // 1. compute density 
            // 2. compute press
            // 3. compute accleration
            // 4. update velocity and position
            // 5. check boundary
            // 6. update block id
            // ...

            Glb::Timer::getInstance().start();
            computeDensityAndPress();
            Glb::Timer::getInstance().recordTime("density and press");
            computeAccleration();
            Glb::Timer::getInstance().recordTime("compute acc");
            eulerIntegration();
            Glb::Timer::getInstance().recordTime("euler intergration");
            boundaryCondition();
            Glb::Timer::getInstance().recordTime("boundary check");
            calculateBlockId();
            Glb::Timer::getInstance().recordTime("renew block id");
        }

        void Solver::computeAccleration()
        {
            float dim = 2.0;
            float constFactor = 2.0 * (dim + 2.0) * Lagrangian2dPara::viscosity;
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticleInfos.size(); i++)
            {

                mPs.mParticleInfos[i].accleration = -glm::vec2(Lagrangian2dPara::gravityX, Lagrangian2dPara::gravityY);
                // std::cout << mPs.mParticleInfos[i].accleration.z << std::endl;
                //  ���� viscosity �� pressure
                glm::vec2 viscosityForce = glm::vec2(0.0);
                glm::vec2 pressureForce = glm::vec2(0.0);
                for (int k = 0; k < mPs.mBlockIdOffs.size(); k++)
                {
                    int bIdj = mPs.mParticleInfos[i].blockId + mPs.mBlockIdOffs[k];

                    if (bIdj >= 0 && bIdj < mPs.mBlockExtens.size())
                    {
                        for (int j = mPs.mBlockExtens[bIdj].x; j < mPs.mBlockExtens[bIdj].y; j++)
                        {
                            if (j == i)
                            {
                                continue;
                            }
                            glm::vec2 radiusIj = mPs.mParticleInfos[i].position - mPs.mParticleInfos[j].position;
                            float diatanceIj = length(radiusIj);
                            if (diatanceIj <= Lagrangian2dPara::supportRadius)
                            {
                                float dotDvToRad = glm::dot(mPs.mParticleInfos[i].velocity - mPs.mParticleInfos[j].velocity, radiusIj);
                                float denom = diatanceIj * diatanceIj + 0.01 * Lagrangian2dPara::supportRadius * Lagrangian2dPara::supportRadius;
                                glm::vec2 wGrad = mW.Grad(radiusIj);
                                // std::cout << wGrad.x << " " << wGrad.y<< std::endl;
                                viscosityForce += (Lagrangian2dPara::density * mPs.mVolume / mPs.mParticleInfos[j].density) * dotDvToRad * wGrad / denom;
                                pressureForce += mPs.mParticleInfos[j].density * (mPs.mParticleInfos[i].pressDivDens2 + mPs.mParticleInfos[j].pressDivDens2) * wGrad;
                            }
                        }
                    }
                }
                mPs.mParticleInfos[i].accleration += viscosityForce * constFactor;
                mPs.mParticleInfos[i].accleration -= pressureForce * mPs.mVolume;
            }
        }

        void Solver::eulerIntegration()
        {
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticleInfos.size(); i++)
            {
                mPs.mParticleInfos[i].velocity = mPs.mParticleInfos[i].velocity + (float)Lagrangian2dPara::dt * mPs.mParticleInfos[i].accleration;
                glm::vec2 newVelocity;
                for (int j = 0; j < 2; j++)
                {
                    newVelocity[j] = max(-Lagrangian2dPara::maxVelocity, min(mPs.mParticleInfos[i].velocity[j], Lagrangian2dPara::maxVelocity));
                }
                mPs.mParticleInfos[i].velocity = newVelocity;
                mPs.mParticleInfos[i].position = mPs.mParticleInfos[i].position + (float)Lagrangian2dPara::dt * mPs.mParticleInfos[i].velocity;
            }
        }

        void Solver::boundaryCondition()
        {
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticleInfos.size(); i++)
            {

                bool invFlag = false;

                if (mPs.mParticleInfos[i].position.x < mPs.mLowerBound.x + Lagrangian2dPara::supportRadius)
                {
                    mPs.mParticleInfos[i].velocity.x = abs(mPs.mParticleInfos[i].velocity.x);
                    invFlag = true;
                }

                if (mPs.mParticleInfos[i].position.y < mPs.mLowerBound.y + Lagrangian2dPara::supportRadius)
                {
                    mPs.mParticleInfos[i].velocity.y = abs(mPs.mParticleInfos[i].velocity.y);
                    invFlag = true;
                }

                if (mPs.mParticleInfos[i].position.x > mPs.mUpperBound.x - Lagrangian2dPara::supportRadius)
                {
                    mPs.mParticleInfos[i].velocity.x = -abs(mPs.mParticleInfos[i].velocity.x);
                    invFlag = true;
                }

                if (mPs.mParticleInfos[i].position.y > mPs.mUpperBound.y - Lagrangian2dPara::supportRadius)
                {
                    mPs.mParticleInfos[i].velocity.y = -abs(mPs.mParticleInfos[i].velocity.y);
                    invFlag = true;
                }

                if (invFlag)
                {
                    mPs.mParticleInfos[i].velocity *= Lagrangian2dPara::velocityAttenuation; // ����߽磬˥���ٶ�
                }

                glm::vec2 newPosition, newVelocity;
                for (int j = 0; j < 2; j++)
                {
                    newPosition[j] = max((mPs.mLowerBound[j] + Lagrangian2dPara::supportRadius + Lagrangian2dPara::eps), min(mPs.mParticleInfos[i].position[j], (mPs.mUpperBound[j] - (Lagrangian2dPara::supportRadius + Lagrangian2dPara::eps))));
                    newVelocity[j] = max(-Lagrangian2dPara::maxVelocity, min(mPs.mParticleInfos[i].velocity[j], Lagrangian2dPara::maxVelocity));
                }
                mPs.mParticleInfos[i].position = newPosition;
                mPs.mParticleInfos[i].velocity = newVelocity;
            }
        }

        void Solver::calculateBlockId()
        {
            for (int i = 0; i < mPs.mParticleInfos.size(); i++)
            {
                glm::vec2 deltePos = mPs.mParticleInfos[i].position - mPs.mLowerBound;
                glm::vec2 blockPosition = glm::floor(deltePos / mPs.mBlockSize);
                mPs.mParticleInfos[i].blockId = blockPosition.y * mPs.mBlockNum.x + blockPosition.x;
            }
        }

        void Solver::computeDensityAndPress()
        {
            for (int i = 0; i < mPs.mParticleInfos.size(); i++)
            {
                for (int k = 0; k < mPs.mBlockIdOffs.size(); k++)
                { // for all neighbor block
                    int bIdj = mPs.mParticleInfos[i].blockId + mPs.mBlockIdOffs[k];
                    if (bIdj >= 0 && bIdj < mPs.mBlockExtens.size())
                    {
                        for (int j = mPs.mBlockExtens[bIdj].x; j < mPs.mBlockExtens[bIdj].y; j++)
                        { // for all neighbor particles
                            if (j == i)
                            {
                                continue;
                            }
                            glm::vec2 radiusIj = mPs.mParticleInfos[i].position - mPs.mParticleInfos[j].position;
                            float diatanceIj = length(radiusIj);
                            if (diatanceIj <= Lagrangian2dPara::supportRadius)
                            {
                                mPs.mParticleInfos[i].density += mW.Value(diatanceIj);
                            }
                        }
                    }
                }

                mPs.mParticleInfos[i].density *= (mPs.mVolume * Lagrangian2dPara::density);
                mPs.mParticleInfos[i].density = max(mPs.mParticleInfos[i].density, Lagrangian2dPara::density);
                mPs.mParticleInfos[i].pressure = Lagrangian2dPara::stiffness * (std::powf(mPs.mParticleInfos[i].density / Lagrangian2dPara::density, Lagrangian2dPara::exponent) - 1.0);
                mPs.mParticleInfos[i].pressDivDens2 = mPs.mParticleInfos[i].pressure / std::powf(mPs.mParticleInfos[i].density, 2);
            }
        }
    }
}
