#include "Lagrangian/include/Solver.h"
#include "Global.h"
#include <iostream>
#include <algorithm>

namespace FluidSimulation
{

    namespace Lagrangian2d
    {
        Solver::Solver(ParticalSystem2d &ps) : mPs(ps), mW(ps.mSupportRadius)
        {
        }

        Solver::~Solver()
        {
        }

        void Solver::solve()
        {
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
            for (int i = 0; i < mPs.mParticalInfos.size(); i++)
            {

                mPs.mParticalInfos[i].accleration = -glm::vec2(Lagrangian2dPara::gravityX, Lagrangian2dPara::gravityY);
                // std::cout << mPs.mParticalInfos[i].accleration.z << std::endl;
                //  ���� viscosity �� pressure
                glm::vec2 viscosityForce = glm::vec2(0.0);
                glm::vec2 pressureForce = glm::vec2(0.0);
                for (int k = 0; k < mPs.mBlockIdOffs.size(); k++)
                {
                    int bIdj = mPs.mParticalInfos[i].blockId + mPs.mBlockIdOffs[k];

                    if (bIdj >= 0 && bIdj < mPs.mBlockExtens.size())
                    {
                        for (int j = mPs.mBlockExtens[bIdj].x; j < mPs.mBlockExtens[bIdj].y; j++)
                        {
                            if (j == i)
                            {
                                continue;
                            }
                            glm::vec2 radiusIj = mPs.mParticalInfos[i].position - mPs.mParticalInfos[j].position;
                            float diatanceIj = length(radiusIj);
                            if (diatanceIj <= Lagrangian2dPara::supportRadius)
                            {
                                float dotDvToRad = glm::dot(mPs.mParticalInfos[i].velocity - mPs.mParticalInfos[j].velocity, radiusIj);
                                float denom = diatanceIj * diatanceIj + 0.01 * Lagrangian2dPara::supportRadius * Lagrangian2dPara::supportRadius;
                                glm::vec2 wGrad = mW.Grad(radiusIj);
                                // std::cout << wGrad.x << " " << wGrad.y<< std::endl;
                                viscosityForce += (Lagrangian2dPara::density * mPs.mVolume / mPs.mParticalInfos[j].density) * dotDvToRad * wGrad / denom;
                                pressureForce += mPs.mParticalInfos[j].density * (mPs.mParticalInfos[i].pressDivDens2 + mPs.mParticalInfos[j].pressDivDens2) * wGrad;
                            }
                        }
                    }
                }
                // std::cout << mPs.mParticalInfos[i].density << std::endl;
                // std::cout << viscosityForce.x << " " << viscosityForce.y << std::endl;
                // std::cout << pressureForce.x << " " << pressureForce.y << std::endl;
                //  ʹ��viscosity �� pressure���¼��ٶ�
                mPs.mParticalInfos[i].accleration += viscosityForce * constFactor;
                // std::cout << mPs.mParticalInfos[i].accleration.y << std::endl;
                mPs.mParticalInfos[i].accleration -= pressureForce * mPs.mVolume;
                // std::cout << mPs.mParticalInfos[i].accleration.x << " " << mPs.mParticalInfos[i].accleration.y << " " << mPs.mParticalInfos[i].accleration.z << std::endl;
            }
        }

        void Solver::eulerIntegration()
        {
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticalInfos.size(); i++)
            {
                mPs.mParticalInfos[i].velocity = mPs.mParticalInfos[i].velocity + (float)Lagrangian2dPara::dt * mPs.mParticalInfos[i].accleration;
                glm::vec2 newVelocity;
                for (int j = 0; j < 2; j++)
                {
                    newVelocity[j] = max(-Lagrangian2dPara::maxVelocity, min(mPs.mParticalInfos[i].velocity[j], Lagrangian2dPara::maxVelocity));
                }
                mPs.mParticalInfos[i].velocity = newVelocity;
                mPs.mParticalInfos[i].position = mPs.mParticalInfos[i].position + (float)Lagrangian2dPara::dt * mPs.mParticalInfos[i].velocity;
            }
        }

        void Solver::boundaryCondition()
        {
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticalInfos.size(); i++)
            {

                bool invFlag = false;

                if (mPs.mParticalInfos[i].position.x < mPs.mLowerBound.x + Lagrangian2dPara::supportRadius)
                {
                    mPs.mParticalInfos[i].velocity.x = abs(mPs.mParticalInfos[i].velocity.x);
                    invFlag = true;
                }

                if (mPs.mParticalInfos[i].position.y < mPs.mLowerBound.y + Lagrangian2dPara::supportRadius)
                {
                    mPs.mParticalInfos[i].velocity.y = abs(mPs.mParticalInfos[i].velocity.y);
                    invFlag = true;
                }

                if (mPs.mParticalInfos[i].position.x > mPs.mUpperBound.x - Lagrangian2dPara::supportRadius)
                {
                    mPs.mParticalInfos[i].velocity.x = -abs(mPs.mParticalInfos[i].velocity.x);
                    invFlag = true;
                }

                if (mPs.mParticalInfos[i].position.y > mPs.mUpperBound.y - Lagrangian2dPara::supportRadius)
                {
                    mPs.mParticalInfos[i].velocity.y = -abs(mPs.mParticalInfos[i].velocity.y);
                    invFlag = true;
                }

                if (invFlag)
                {
                    mPs.mParticalInfos[i].velocity *= Lagrangian2dPara::velocityAttenuation; // ����߽磬˥���ٶ�
                }

                // �����ٶȺ�λ��
                glm::vec2 newPosition, newVelocity;
                for (int j = 0; j < 2; j++)
                {
                    newPosition[j] = max((mPs.mLowerBound[j] + Lagrangian2dPara::supportRadius + Lagrangian2dPara::eps), min(mPs.mParticalInfos[i].position[j], (mPs.mUpperBound[j] - (Lagrangian2dPara::supportRadius + Lagrangian2dPara::eps))));
                    newVelocity[j] = max(-Lagrangian2dPara::maxVelocity, min(mPs.mParticalInfos[i].velocity[j], Lagrangian2dPara::maxVelocity));
                }
                mPs.mParticalInfos[i].position = newPosition;
                mPs.mParticalInfos[i].velocity = newVelocity;
            }
        }

        void Solver::calculateBlockId()
        {
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticalInfos.size(); i++)
            {
                glm::vec2 deltePos = mPs.mParticalInfos[i].position - mPs.mLowerBound;
                glm::vec2 blockPosition = glm::floor(deltePos / mPs.mBlockSize);
                mPs.mParticalInfos[i].blockId = blockPosition.y * mPs.mBlockNum.x + blockPosition.x;
            }
        }

        void Solver::computeDensityAndPress()
        {
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticalInfos.size(); i++)
            {
                for (int k = 0; k < mPs.mBlockIdOffs.size(); k++)
                { // for all neighbor block
                    int bIdj = mPs.mParticalInfos[i].blockId + mPs.mBlockIdOffs[k];
                    if (bIdj >= 0 && bIdj < mPs.mBlockExtens.size())
                    {
                        for (int j = mPs.mBlockExtens[bIdj].x; j < mPs.mBlockExtens[bIdj].y; j++)
                        { // for all neighbor particals
                            if (j == i)
                            {
                                continue;
                            }
                            glm::vec2 radiusIj = mPs.mParticalInfos[i].position - mPs.mParticalInfos[j].position;
                            float diatanceIj = length(radiusIj);
                            if (diatanceIj <= Lagrangian2dPara::supportRadius)
                            {
                                mPs.mParticalInfos[i].density += mW.Value(diatanceIj);
                            }
                        }
                    }
                }

                mPs.mParticalInfos[i].density *= (mPs.mVolume * Lagrangian2dPara::density);
                mPs.mParticalInfos[i].density = max(mPs.mParticalInfos[i].density, Lagrangian2dPara::density);
                mPs.mParticalInfos[i].pressure = Lagrangian2dPara::stiffness * (std::powf(mPs.mParticalInfos[i].density / Lagrangian2dPara::density, Lagrangian2dPara::exponent) - 1.0);
                mPs.mParticalInfos[i].pressDivDens2 = mPs.mParticalInfos[i].pressure / std::powf(mPs.mParticalInfos[i].density, 2);
            }
        }
    }
}
