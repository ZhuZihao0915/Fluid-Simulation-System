#include "fluid3d/SPH/include/Solver.h"
#include <omp.h>

namespace FluidSimulation {

	namespace SPH3d {
		Solver::Solver(ParticalSystem3d& ps) : mPs(ps), mW(ps.mSupportRadius)
		{

		}

		Solver::~Solver() {

		}

		void Solver::solve() {
			// TODO
			// 求解/模拟
			computeDensityAndPress();
			computeAccleration();
			eulerIntegration();
			boundaryCondition();
			calculateBlockId();
		}

		// 计算加速度
		void Solver::computeAccleration() {
			float dim = 3.0;
			float constFactor = 2.0 * (dim + 2.0) * mPs.mViscosity;
#pragma omp parallel for
			for (int i = 0; i < mPs.mParticalInfos.size(); i++) {

				mPs.mParticalInfos[i].accleration = SPH3dPara::gravity * -Glb::Z_AXIS;
				//std::cout << mPs.mParticalInfos[i].accleration.z << std::endl;
				// 计算 viscosity 和 pressure
				glm::vec3 viscosityForce = glm::vec3(0.0);
				glm::vec3 pressureForce = glm::vec3(0.0);
				for (int k = 0; k < mPs.mBlockIdOffs.size(); k++) {   
					int bIdj = mPs.mParticalInfos[i].blockId + mPs.mBlockIdOffs[k];
					if (bIdj >= 0 && bIdj < mPs.mBlockExtens.size()) {
						for (int j = mPs.mBlockExtens[bIdj].x; j < mPs.mBlockExtens[bIdj].y; j++) {
							glm::vec3 radiusIj = mPs.mParticalInfos[i].position - mPs.mParticalInfos[j].position;
							float diatanceIj = length(radiusIj);
							if (diatanceIj <= SPH3dPara::supportRadius) {
								float dotDvToRad = glm::dot(mPs.mParticalInfos[i].velocity - mPs.mParticalInfos[j].velocity, radiusIj);
								float denom = diatanceIj * diatanceIj + 0.01 * SPH3dPara::supportRadius * SPH3dPara::supportRadius;
								glm::vec3 wGrad = mW.GetGrad(diatanceIj / SPH3dPara::supportRadius).g * radiusIj;
								//std::cout << wGrad.z << std::endl;
								viscosityForce += (float)(0.5 / mPs.mParticalInfos[j].density) * dotDvToRad * wGrad / denom;
								pressureForce += mPs.mParticalInfos[j].density * (mPs.mParticalInfos[i].pressDivDens2 + mPs.mParticalInfos[j].pressDivDens2) * wGrad;
							}
						}
					}
				}
				//std::cout << viscosityForce.x << " " << viscosityForce.y << " " << viscosityForce.z << std::endl;
				// 使用viscosity 和 pressure更新加速度
				mPs.mParticalInfos[i].accleration += viscosityForce * constFactor;
				// std::cout << mPs.mParticalInfos[i].accleration.z << std::endl;
				mPs.mParticalInfos[i].accleration -= pressureForce * mPs.mVolume;
				// std::cout << mPs.mParticalInfos[i].accleration.x << " " << mPs.mParticalInfos[i].accleration.y << " " << mPs.mParticalInfos[i].accleration.z << std::endl;
			}
		}

		void Solver::eulerIntegration() {
#pragma omp parallel for
			for (int i = 0; i < mPs.mParticalInfos.size(); i++) {
				// 使用加速度（和dt）更新速度
				mPs.mParticalInfos[i].velocity = mPs.mParticalInfos[i].velocity + SPH3dPara::dt * mPs.mParticalInfos[i].accleration;
				// 限制速度在各方向的大小
				glm::vec3 newVelocity;
				for (int j = 0; j < 3; j++) {
					newVelocity[j] = max(-SPH3dPara::maxVelocity, min(mPs.mParticalInfos[i].velocity[j], SPH3dPara::maxVelocity));
				}
				mPs.mParticalInfos[i].velocity = newVelocity;
				// 使用速度（和dt）更新位置
				mPs.mParticalInfos[i].position = mPs.mParticalInfos[i].position + SPH3dPara::dt * mPs.mParticalInfos[i].velocity;
			}
		}



		void Solver::boundaryCondition() {
#pragma omp parallel for
			for (int i = 0; i < mPs.mParticalInfos.size(); i++) {

				bool invFlag = false;

				if (mPs.mParticalInfos[i].position.x < mPs.mLowerBound.x + SPH3dPara::supportRadius) {
					mPs.mParticalInfos[i].velocity.x = abs(mPs.mParticalInfos[i].velocity.x);
					invFlag = true;
				}

				if (mPs.mParticalInfos[i].position.y < mPs.mLowerBound.y + SPH3dPara::supportRadius) {
					mPs.mParticalInfos[i].velocity.y = abs(mPs.mParticalInfos[i].velocity.y);
					invFlag = true;
				}

				if (mPs.mParticalInfos[i].position.z < mPs.mLowerBound.z + SPH3dPara::supportRadius) {
					mPs.mParticalInfos[i].velocity.z = abs(mPs.mParticalInfos[i].velocity.z);
					invFlag = true;
				}

				if (mPs.mParticalInfos[i].position.x > mPs.mUpperBound.x - SPH3dPara::supportRadius) {
					mPs.mParticalInfos[i].velocity.x = -abs(mPs.mParticalInfos[i].velocity.x);
					invFlag = true;
				}

				if (mPs.mParticalInfos[i].position.y > mPs.mUpperBound.y - SPH3dPara::supportRadius) {
					mPs.mParticalInfos[i].velocity.y = -abs(mPs.mParticalInfos[i].velocity.y);
					invFlag = true;
				}

				if (mPs.mParticalInfos[i].position.z > mPs.mUpperBound.z - SPH3dPara::supportRadius) {
					mPs.mParticalInfos[i].velocity.z = -abs(mPs.mParticalInfos[i].velocity.z);
					invFlag = true;
				}

				if (invFlag) {
					mPs.mParticalInfos[i].velocity *= SPH3dPara::velocityAttenuation;	// 到达边界，衰减速度
				}

				// 限制速度和位置
				glm::vec3 newPosition, newVelocity;
				for (int j = 0; j < 3; j++) {
					newPosition[j] = max((mPs.mLowerBound[j] + SPH3dPara::supportRadius + SPH3dPara::eps), min(mPs.mParticalInfos[i].position[j], (mPs.mUpperBound[j] - (SPH3dPara::supportRadius + SPH3dPara::eps))));
					newVelocity[j] = max(-SPH3dPara::maxVelocity, min(mPs.mParticalInfos[i].velocity[j], SPH3dPara::maxVelocity));
				}
				mPs.mParticalInfos[i].position = newPosition;
				mPs.mParticalInfos[i].velocity = newVelocity;

			}

		}

		void Solver::calculateBlockId() {
#pragma omp parallel for
			for (int i = 0; i < mPs.mParticalInfos.size(); i++) {
				// 相对于容器原点的位置
				glm::vec3 deltePos = mPs.mParticalInfos[i].position - mPs.mLowerBound;
				// 计算块的位置
				glm::vec3 blockPosition = glm::floor(deltePos / mPs.mBlockSize);
				// 更新块的ID
				mPs.mParticalInfos[i].blockId = blockPosition.z * mPs.mBlockNum.x * mPs.mBlockNum.y + blockPosition.y * mPs.mBlockNum.x + blockPosition.x;
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
							// 根据粒子之间的距离，模拟密度
							glm::vec3 radiusIj = mPs.mParticalInfos[i].position - mPs.mParticalInfos[j].position;
							float diatanceIj = length(radiusIj);
							if (diatanceIj <= SPH3dPara::supportRadius) {
								mPs.mParticalInfos[i].density += mW.GetGrad(diatanceIj / SPH3dPara::supportRadius).r;
							}
						}
					}
				}
				
				mPs.mParticalInfos[i].density *= (mPs.mVolume * SPH3dPara::density0);
				mPs.mParticalInfos[i].density = max(mPs.mParticalInfos[i].density, SPH3dPara::density0);
				// 根据更新得到的密度，以及设定的stiffness、exponent参数计算得到pressure
				mPs.mParticalInfos[i].pressure = SPH3dPara::stiffness * (pow(mPs.mParticalInfos[i].density / SPH3dPara::density0, SPH3dPara::exponent) - 1.0);
				mPs.mParticalInfos[i].pressDivDens2 = mPs.mParticalInfos[i].pressure / pow(mPs.mParticalInfos[i].density, 2);
			}
		}
	}

}