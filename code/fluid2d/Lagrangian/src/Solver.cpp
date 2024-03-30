#include "Lagrangian/include/Solver.h"
#include "Global.h"
#include <iostream>
#include <algorithm>

namespace FluidSimulation
{

    namespace Lagrangian2d
    {
        Solver::Solver(ParticleSystem2d &ps) : mPs(ps), mW(ps.supportRadius)
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
            for (int i = 0; i < mPs.particles.size(); i++)
            {

                mPs.particles[i].accleration = -glm::vec2(Lagrangian2dPara::gravityX, Lagrangian2dPara::gravityY);
                glm::vec2 viscosityForce = glm::vec2(0.0);
                glm::vec2 pressureForce = glm::vec2(0.0);
                for (int k = 0; k < mPs.blockIdOffs.size(); k++)
                {
                    int bIdj = mPs.particles[i].blockId + mPs.blockIdOffs[k];

                    if (bIdj >= 0 && bIdj < mPs.blockExtens.size())
                    {
                        for (int j = mPs.blockExtens[bIdj].x; j < mPs.blockExtens[bIdj].y; j++)
                        {
                            if (j == i)
                            {
                                continue;
                            }
                            glm::vec2 radiusIj = mPs.particles[i].position - mPs.particles[j].position;
                            float diatanceIj = length(radiusIj);
                            if (diatanceIj <= Lagrangian2dPara::supportRadius)
                            {
                                float dotDvToRad = glm::dot(mPs.particles[i].velocity - mPs.particles[j].velocity, radiusIj);
                                float denom = diatanceIj * diatanceIj + 0.01 * Lagrangian2dPara::supportRadius * Lagrangian2dPara::supportRadius;
                                glm::vec2 wGrad = mW.Grad(radiusIj);
                                // std::cout << wGrad.x << " " << wGrad.y<< std::endl;
                                viscosityForce += (Lagrangian2dPara::density * mPs.particleVolume / mPs.particles[j].density) * dotDvToRad * wGrad / denom;
                                pressureForce += mPs.particles[j].density * (mPs.particles[i].pressDivDens2 + mPs.particles[j].pressDivDens2) * wGrad;
                            }
                        }
                    }
                }
                mPs.particles[i].accleration += viscosityForce * constFactor;
                mPs.particles[i].accleration -= pressureForce * mPs.particleVolume;
            }
        }

        void Solver::eulerIntegration()
        {
            for (int i = 0; i < mPs.particles.size(); i++)
            {
                mPs.particles[i].velocity = mPs.particles[i].velocity + (float)Lagrangian2dPara::dt * mPs.particles[i].accleration;
                glm::vec2 newVelocity;
                for (int j = 0; j < 2; j++)
                {
                    newVelocity[j] = max(-Lagrangian2dPara::maxVelocity, min(mPs.particles[i].velocity[j], Lagrangian2dPara::maxVelocity));
                }
                mPs.particles[i].velocity = newVelocity;
                mPs.particles[i].position = mPs.particles[i].position + (float)Lagrangian2dPara::dt * mPs.particles[i].velocity;
            }
        }

        void Solver::boundaryCondition()
        {
            for (int i = 0; i < mPs.particles.size(); i++)
            {

                bool invFlag = false;

                if (mPs.particles[i].position.x < mPs.lowerBound.x + Lagrangian2dPara::supportRadius)
                {
                    mPs.particles[i].velocity.x = abs(mPs.particles[i].velocity.x);
                    invFlag = true;
                }

                if (mPs.particles[i].position.y < mPs.lowerBound.y + Lagrangian2dPara::supportRadius)
                {
                    mPs.particles[i].velocity.y = abs(mPs.particles[i].velocity.y);
                    invFlag = true;
                }

                if (mPs.particles[i].position.x > mPs.upperBound.x - Lagrangian2dPara::supportRadius)
                {
                    mPs.particles[i].velocity.x = -abs(mPs.particles[i].velocity.x);
                    invFlag = true;
                }

                if (mPs.particles[i].position.y > mPs.upperBound.y - Lagrangian2dPara::supportRadius)
                {
                    mPs.particles[i].velocity.y = -abs(mPs.particles[i].velocity.y);
                    invFlag = true;
                }

                if (invFlag)
                {
                    mPs.particles[i].velocity *= Lagrangian2dPara::velocityAttenuation;
                }

                glm::vec2 newPosition, newVelocity;
                for (int j = 0; j < 2; j++)
                {
                    newPosition[j] = max((mPs.lowerBound[j] + Lagrangian2dPara::supportRadius + Lagrangian2dPara::eps), min(mPs.particles[i].position[j], (mPs.upperBound[j] - (Lagrangian2dPara::supportRadius + Lagrangian2dPara::eps))));
                    newVelocity[j] = max(-Lagrangian2dPara::maxVelocity, min(mPs.particles[i].velocity[j], Lagrangian2dPara::maxVelocity));
                }
                mPs.particles[i].position = newPosition;
                mPs.particles[i].velocity = newVelocity;
            }
        }

        void Solver::calculateBlockId()
        {
            for (int i = 0; i < mPs.particles.size(); i++)
            {
                glm::vec2 deltePos = mPs.particles[i].position - mPs.lowerBound;
                glm::vec2 blockPosition = glm::floor(deltePos / mPs.blockSize);
                mPs.particles[i].blockId = blockPosition.y * mPs.blockNum.x + blockPosition.x;
            }
        }

        void Solver::computeDensityAndPress()
        {
            for (int i = 0; i < mPs.particles.size(); i++)
            {
                for (int k = 0; k < mPs.blockIdOffs.size(); k++)
                { // for all neighbor block
                    int bIdj = mPs.particles[i].blockId + mPs.blockIdOffs[k];
                    if (bIdj >= 0 && bIdj < mPs.blockExtens.size())
                    {
                        for (int j = mPs.blockExtens[bIdj].x; j < mPs.blockExtens[bIdj].y; j++)
                        { // for all neighbor particles
                            if (j == i)
                            {
                                continue;
                            }
                            glm::vec2 radiusIj = mPs.particles[i].position - mPs.particles[j].position;
                            float diatanceIj = length(radiusIj);
                            if (diatanceIj <= Lagrangian2dPara::supportRadius)
                            {
                                mPs.particles[i].density += mW.Value(diatanceIj);
                            }
                        }
                    }
                }

                mPs.particles[i].density *= (mPs.particleVolume * Lagrangian2dPara::density);
                mPs.particles[i].density = max(mPs.particles[i].density, Lagrangian2dPara::density);
                mPs.particles[i].pressure = Lagrangian2dPara::stiffness * (std::powf(mPs.particles[i].density / Lagrangian2dPara::density, Lagrangian2dPara::exponent) - 1.0);
                mPs.particles[i].pressDivDens2 = mPs.particles[i].pressure / std::powf(mPs.particles[i].density, 2);
            }
        }
    }
}
