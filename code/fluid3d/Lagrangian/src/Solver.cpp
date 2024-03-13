#include "fluid3d/Lagrangian/include/Solver.h"
#include <omp.h>

namespace FluidSimulation
{

	namespace Lagrangian3d
	{
		Solver::Solver(ParticalSystem3d &ps) : mPs(ps), mW(ps.mSupportRadius)
		{
		}

		Solver::~Solver()
		{
		}

		void Solver::solve()
		{
			// TODO
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
			float dim = 3.0;
			float constFactor = 2.0 * (dim + 2.0) * Lagrangian3dPara::viscosity;
#pragma omp parallel for
			for (int i = 0; i < mPs.mParticalInfos.size(); i++)
			{

				mPs.mParticalInfos[i].accleration = glm::vec3(-Lagrangian3dPara::gravityX, -Lagrangian3dPara::gravityY, -Lagrangian3dPara::gravityZ);
				// std::cout << mPs.mParticalInfos[i].accleration.z << std::endl;
				// update viscosity and pressure
				glm::vec3 viscosityForce = glm::vec3(0.0);
				glm::vec3 pressureForce = glm::vec3(0.0);
				for (int k = 0; k < mPs.mBlockIdOffs.size(); k++)
				{
					int bIdj = mPs.mParticalInfos[i].blockId + mPs.mBlockIdOffs[k];
					if (bIdj >= 0 && bIdj < mPs.mBlockExtens.size())
					{
						for (int j = mPs.mBlockExtens[bIdj].x; j < mPs.mBlockExtens[bIdj].y; j++)
						{
							glm::vec3 radiusIj = mPs.mParticalInfos[i].position - mPs.mParticalInfos[j].position;
							float diatanceIj = length(radiusIj);
							if (diatanceIj <= Lagrangian3dPara::supportRadius)
							{
								float dotDvToRad = glm::dot(mPs.mParticalInfos[i].velocity - mPs.mParticalInfos[j].velocity, radiusIj);
								float denom = diatanceIj * diatanceIj + 0.01 * Lagrangian3dPara::supportRadius * Lagrangian3dPara::supportRadius;
								glm::vec3 wGrad = mW.GetGrad(diatanceIj / Lagrangian3dPara::supportRadius).g * radiusIj;
								// std::cout << wGrad.z << std::endl;
								viscosityForce += (float)(0.5 / mPs.mParticalInfos[j].density) * dotDvToRad * wGrad / denom;
								pressureForce += mPs.mParticalInfos[j].density * (mPs.mParticalInfos[i].pressDivDens2 + mPs.mParticalInfos[j].pressDivDens2) * wGrad;
							}
						}
					}
				}
				// std::cout << viscosityForce.x << " " << viscosityForce.y << " " << viscosityForce.z << std::endl;
				mPs.mParticalInfos[i].accleration += viscosityForce * constFactor;
				// std::cout << mPs.mParticalInfos[i].accleration.z << std::endl;
				mPs.mParticalInfos[i].accleration -= pressureForce * mPs.mVolume;
			}
		}

		void Solver::eulerIntegration()
		{
#pragma omp parallel for
			for (int i = 0; i < mPs.mParticalInfos.size(); i++)
			{
				mPs.mParticalInfos[i].velocity = mPs.mParticalInfos[i].velocity + Lagrangian3dPara::dt * mPs.mParticalInfos[i].accleration;
				glm::vec3 newVelocity;
				for (int j = 0; j < 3; j++)
				{
					newVelocity[j] = max(-Lagrangian3dPara::maxVelocity, min(mPs.mParticalInfos[i].velocity[j], Lagrangian3dPara::maxVelocity));
				}
				mPs.mParticalInfos[i].velocity = newVelocity;
				mPs.mParticalInfos[i].position = mPs.mParticalInfos[i].position + Lagrangian3dPara::dt * mPs.mParticalInfos[i].velocity;
			}
		}

		void Solver::boundaryCondition()
		{
#pragma omp parallel for
			for (int i = 0; i < mPs.mParticalInfos.size(); i++)
			{

				bool invFlag = false;

				if (mPs.mParticalInfos[i].position.x < mPs.mLowerBound.x + Lagrangian3dPara::supportRadius)
				{
					mPs.mParticalInfos[i].velocity.x = abs(mPs.mParticalInfos[i].velocity.x);
					invFlag = true;
				}

				if (mPs.mParticalInfos[i].position.y < mPs.mLowerBound.y + Lagrangian3dPara::supportRadius)
				{
					mPs.mParticalInfos[i].velocity.y = abs(mPs.mParticalInfos[i].velocity.y);
					invFlag = true;
				}

				if (mPs.mParticalInfos[i].position.z < mPs.mLowerBound.z + Lagrangian3dPara::supportRadius)
				{
					mPs.mParticalInfos[i].velocity.z = abs(mPs.mParticalInfos[i].velocity.z);
					invFlag = true;
				}

				if (mPs.mParticalInfos[i].position.x > mPs.mUpperBound.x - Lagrangian3dPara::supportRadius)
				{
					mPs.mParticalInfos[i].velocity.x = -abs(mPs.mParticalInfos[i].velocity.x);
					invFlag = true;
				}

				if (mPs.mParticalInfos[i].position.y > mPs.mUpperBound.y - Lagrangian3dPara::supportRadius)
				{
					mPs.mParticalInfos[i].velocity.y = -abs(mPs.mParticalInfos[i].velocity.y);
					invFlag = true;
				}

				if (mPs.mParticalInfos[i].position.z > mPs.mUpperBound.z - Lagrangian3dPara::supportRadius)
				{
					mPs.mParticalInfos[i].velocity.z = -abs(mPs.mParticalInfos[i].velocity.z);
					invFlag = true;
				}

				if (invFlag)
				{
					mPs.mParticalInfos[i].velocity *= Lagrangian3dPara::velocityAttenuation; // ����߽磬˥���ٶ�
				}

				// �����ٶȺ�λ��
				glm::vec3 newPosition, newVelocity;
				for (int j = 0; j < 3; j++)
				{
					newPosition[j] = max((mPs.mLowerBound[j] + Lagrangian3dPara::supportRadius + Lagrangian3dPara::eps), min(mPs.mParticalInfos[i].position[j], (mPs.mUpperBound[j] - (Lagrangian3dPara::supportRadius + Lagrangian3dPara::eps))));
					newVelocity[j] = max(-Lagrangian3dPara::maxVelocity, min(mPs.mParticalInfos[i].velocity[j], Lagrangian3dPara::maxVelocity));
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
				glm::vec3 deltePos = mPs.mParticalInfos[i].position - mPs.mLowerBound;
				glm::vec3 blockPosition = glm::floor(deltePos / mPs.mBlockSize);
				mPs.mParticalInfos[i].blockId = blockPosition.z * mPs.mBlockNum.x * mPs.mBlockNum.y + blockPosition.y * mPs.mBlockNum.x + blockPosition.x;
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
							glm::vec3 radiusIj = mPs.mParticalInfos[i].position - mPs.mParticalInfos[j].position;
							float diatanceIj = length(radiusIj);
							if (diatanceIj <= Lagrangian3dPara::supportRadius)
							{
								mPs.mParticalInfos[i].density += mW.GetGrad(diatanceIj / Lagrangian3dPara::supportRadius).r;
							}
						}
					}
				}

				mPs.mParticalInfos[i].density *= (mPs.mVolume * Lagrangian3dPara::density0);
				mPs.mParticalInfos[i].density = max(mPs.mParticalInfos[i].density, Lagrangian3dPara::density0);
				mPs.mParticalInfos[i].pressure = Lagrangian3dPara::stiffness * (pow(mPs.mParticalInfos[i].density / Lagrangian3dPara::density0, Lagrangian3dPara::exponent) - 1.0);
				mPs.mParticalInfos[i].pressDivDens2 = mPs.mParticalInfos[i].pressure / pow(mPs.mParticalInfos[i].density, 2);
			}
		}
	}

}