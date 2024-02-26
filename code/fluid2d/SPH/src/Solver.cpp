#include "SPH/include/Solver.h"
#include "Global.h"
#include <iostream>
#include <algorithm>

namespace FluidSimulation {

    namespace SPH2d {
        Solver::Solver(ParticalSystem2d& ps) : mPs(ps), mW(ps.mSupportRadius)
        {

        }

        Solver::~Solver() {

        }

        void Solver::solve() {
            computeDensityAndPress();
            computeAccleration();
            eulerIntegration();
            boundaryCondition();
            calculateBlockId();
        }

        void Solver::computeAccleration(){
            float dim = 2.0;
            float constFactor = 2.0 * (dim + 2.0) * mPs.mViscosity;
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticalInfos.size(); i++) {

                mPs.mParticalInfos[i].accleration = SPH2dPara::gravity * -glm::vec2(0.0f, 1.0f);
                //std::cout << mPs.mParticalInfos[i].accleration.z << std::endl;
                // 计算 viscosity 和 pressure
                glm::vec2 viscosityForce = glm::vec2(0.0);
                glm::vec2 pressureForce = glm::vec2(0.0);
                for (int k = 0; k < mPs.mBlockIdOffs.size(); k++) {
                    int bIdj = mPs.mParticalInfos[i].blockId + mPs.mBlockIdOffs[k];
                    
                    if (bIdj >= 0 && bIdj < mPs.mBlockExtens.size()) {
                        for (int j = mPs.mBlockExtens[bIdj].x; j < mPs.mBlockExtens[bIdj].y; j++) {
                            if (j == i) { continue; }
                            glm::vec2 radiusIj = mPs.mParticalInfos[i].position - mPs.mParticalInfos[j].position;
                            float diatanceIj = length(radiusIj);
                            if (diatanceIj <= SPH2dPara::supportRadius) {
                                float dotDvToRad = glm::dot(mPs.mParticalInfos[i].velocity - mPs.mParticalInfos[j].velocity, radiusIj);
                                float denom = diatanceIj * diatanceIj + 0.01 * SPH2dPara::supportRadius * SPH2dPara::supportRadius;
                                glm::vec2 wGrad = mW.Grad(radiusIj);
                                //std::cout << wGrad.x << " " << wGrad.y<< std::endl;
                                viscosityForce += (mPs.mMass / mPs.mParticalInfos[j].density) * dotDvToRad * wGrad / denom;
                                pressureForce += mPs.mParticalInfos[j].density * (mPs.mParticalInfos[i].pressDivDens2 + mPs.mParticalInfos[j].pressDivDens2) * wGrad;
                            }
                        }
                    }
                }
                //std::cout << mPs.mParticalInfos[i].density << std::endl;
                //std::cout << viscosityForce.x << " " << viscosityForce.y << std::endl;
                //std::cout << pressureForce.x << " " << pressureForce.y << std::endl;
                // 使用viscosity 和 pressure更新加速度
                mPs.mParticalInfos[i].accleration += viscosityForce * constFactor;
                //std::cout << mPs.mParticalInfos[i].accleration.y << std::endl;
                mPs.mParticalInfos[i].accleration -= pressureForce * mPs.mVolume;
                // std::cout << mPs.mParticalInfos[i].accleration.x << " " << mPs.mParticalInfos[i].accleration.y << " " << mPs.mParticalInfos[i].accleration.z << std::endl;
            }
        }

        void Solver::eulerIntegration() {
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticalInfos.size(); i++) {
                // 使用加速度（和dt）更新速度
                mPs.mParticalInfos[i].velocity = mPs.mParticalInfos[i].velocity + SPH2dPara::dt * mPs.mParticalInfos[i].accleration;
                // 限制速度在各方向的大小
                glm::vec2 newVelocity;
                for (int j = 0; j < 2; j++) {
                    newVelocity[j] = max(-SPH2dPara::maxVelocity, min(mPs.mParticalInfos[i].velocity[j], SPH2dPara::maxVelocity));
                }
                mPs.mParticalInfos[i].velocity = newVelocity;
                // 使用速度（和dt）更新位置
                mPs.mParticalInfos[i].position = mPs.mParticalInfos[i].position + SPH2dPara::dt * mPs.mParticalInfos[i].velocity;
            }
        }

        void Solver::boundaryCondition() {
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticalInfos.size(); i++) {

                bool invFlag = false;

                if (mPs.mParticalInfos[i].position.x < mPs.mLowerBound.x + SPH2dPara::supportRadius) {
                    mPs.mParticalInfos[i].velocity.x = abs(mPs.mParticalInfos[i].velocity.x);
                    invFlag = true;
                }

                if (mPs.mParticalInfos[i].position.y < mPs.mLowerBound.y + SPH2dPara::supportRadius) {
                    mPs.mParticalInfos[i].velocity.y = abs(mPs.mParticalInfos[i].velocity.y);
                    invFlag = true;
                }

                if (mPs.mParticalInfos[i].position.x > mPs.mUpperBound.x - SPH2dPara::supportRadius) {
                    mPs.mParticalInfos[i].velocity.x = -abs(mPs.mParticalInfos[i].velocity.x);
                    invFlag = true;
                }

                if (mPs.mParticalInfos[i].position.y > mPs.mUpperBound.y - SPH2dPara::supportRadius) {
                    mPs.mParticalInfos[i].velocity.y = -abs(mPs.mParticalInfos[i].velocity.y);
                    invFlag = true;
                }

                if (invFlag) {
                    mPs.mParticalInfos[i].velocity *= SPH2dPara::velocityAttenuation;	// 到达边界，衰减速度
                }

                // 限制速度和位置
                glm::vec2 newPosition, newVelocity;
                for (int j = 0; j < 2; j++) {
                    newPosition[j] = max((mPs.mLowerBound[j] + SPH2dPara::supportRadius + SPH2dPara::eps), min(mPs.mParticalInfos[i].position[j], (mPs.mUpperBound[j] - (SPH2dPara::supportRadius + SPH2dPara::eps))));
                    newVelocity[j] = max(-SPH2dPara::maxVelocity, min(mPs.mParticalInfos[i].velocity[j], SPH2dPara::maxVelocity));
                }
                mPs.mParticalInfos[i].position = newPosition;
                mPs.mParticalInfos[i].velocity = newVelocity;

            }

        }

        void Solver::calculateBlockId() {
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticalInfos.size(); i++) {
                // 相对于容器原点的位置
                glm::vec2 deltePos = mPs.mParticalInfos[i].position - mPs.mLowerBound;
                // 计算块的位置
                glm::vec2 blockPosition = glm::floor(deltePos / mPs.mBlockSize);
                // 更新块的ID
                mPs.mParticalInfos[i].blockId = blockPosition.y * mPs.mBlockNum.x + blockPosition.x;
            }
        }

        void Solver::computeDensityAndPress() {
#pragma omp parallel for
            for (int i = 0; i < mPs.mParticalInfos.size(); i++) {
                // 遍历邻近的块
                for (int k = 0; k < mPs.mBlockIdOffs.size(); k++) {     // for all neighbor block
                    int bIdj = mPs.mParticalInfos[i].blockId + mPs.mBlockIdOffs[k];
                    if (bIdj >= 0 && bIdj < mPs.mBlockExtens.size()) {
                        // 遍历块中的粒子
                        for (int j = mPs.mBlockExtens[bIdj].x; j < mPs.mBlockExtens[bIdj].y; j++) {   // for all neighbor particals
                            if (j == i) { continue; }
                            // 根据粒子之间的距离，模拟密度
                            glm::vec2 radiusIj = mPs.mParticalInfos[i].position - mPs.mParticalInfos[j].position;
                            float diatanceIj = length(radiusIj);
                            if (diatanceIj <= SPH2dPara::supportRadius) {
                                mPs.mParticalInfos[i].density += mW.Value(diatanceIj);
                            }
                        }
                    }
                }

                mPs.mParticalInfos[i].density *= (mPs.mVolume * SPH2dPara::density);
                mPs.mParticalInfos[i].density = max(mPs.mParticalInfos[i].density, SPH2dPara::density);
                //std::cout << mPs.mParticalInfos[i].density << std::endl << std::endl;
                // 根据更新得到的密度，以及设定的stiffness、exponent参数计算得到pressure
                mPs.mParticalInfos[i].pressure = SPH2dPara::stiffness * (std::powf(mPs.mParticalInfos[i].density / SPH2dPara::density, SPH2dPara::exponent) - 1.0);
                mPs.mParticalInfos[i].pressDivDens2 = mPs.mParticalInfos[i].pressure / std::powf(mPs.mParticalInfos[i].density, 2);
            }
        }
    }
}

