#include "fluid3d/Lagrangian/include/Solver.h"
#include <omp.h>

namespace FluidSimulation
{

	namespace Lagrangian3d
	{
		Solver::Solver(ParticleSystem3d &ps) : mPs(ps), mW(ps.supportRadius)
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
			float dim = 3.0;
			float constFactor = 2.0 * (dim + 2.0) * Lagrangian3dPara::viscosity;
			for (int i = 0; i < mPs.particles.size(); i++)
			{

				mPs.particles[i].accleration = glm::vec3(-Lagrangian3dPara::gravityX, -Lagrangian3dPara::gravityY, -Lagrangian3dPara::gravityZ);
				// std::cout << mPs.mParticleInfos[i].accleration.z << std::endl;
				// update viscosity and pressure
				glm::vec3 viscosityForce = glm::vec3(0.0);
				glm::vec3 pressureForce = glm::vec3(0.0);
				for (int k = 0; k < mPs.blockIdOffs.size(); k++)
				{
					int bIdj = mPs.particles[i].blockId + mPs.blockIdOffs[k];
					if (bIdj >= 0 && bIdj < mPs.blockExtens.size())
					{
						for (int j = mPs.blockExtens[bIdj].x; j < mPs.blockExtens[bIdj].y; j++)
						{
							glm::vec3 radiusIj = mPs.particles[i].position - mPs.particles[j].position;
							float diatanceIj = length(radiusIj);
							if (diatanceIj <= Lagrangian3dPara::supportRadius)
							{
								float dotDvToRad = glm::dot(mPs.particles[i].velocity - mPs.particles[j].velocity, radiusIj);
								float denom = diatanceIj * diatanceIj + 0.01 * Lagrangian3dPara::supportRadius * Lagrangian3dPara::supportRadius;
								glm::vec3 wGrad = mW.GetValueAndGrad(diatanceIj / Lagrangian3dPara::supportRadius).g * radiusIj;
								// std::cout << wGrad.z << std::endl;
								viscosityForce += (float)(0.5 / mPs.particles[j].density) * dotDvToRad * wGrad / denom;
								pressureForce += mPs.particles[j].density * (mPs.particles[i].pressDivDens2 + mPs.particles[j].pressDivDens2) * wGrad;
							}
						}
					}
				}
				// std::cout << viscosityForce.x << " " << viscosityForce.y << " " << viscosityForce.z << std::endl;
				mPs.particles[i].accleration += viscosityForce * constFactor;
				// std::cout << mPs.mParticleInfos[i].accleration.z << std::endl;
				mPs.particles[i].accleration -= pressureForce * mPs.particleVolume;
			}
		}

		void Solver::eulerIntegration()
		{
			for (int i = 0; i < mPs.particles.size(); i++)
			{
				mPs.particles[i].velocity = mPs.particles[i].velocity + Lagrangian3dPara::dt * mPs.particles[i].accleration;
				glm::vec3 newVelocity;
				for (int j = 0; j < 3; j++)
				{
					newVelocity[j] = max(-Lagrangian3dPara::maxVelocity, min(mPs.particles[i].velocity[j], Lagrangian3dPara::maxVelocity));
				}
				mPs.particles[i].velocity = newVelocity;
				mPs.particles[i].position = mPs.particles[i].position + Lagrangian3dPara::dt * mPs.particles[i].velocity;
			}
		}

		void Solver::boundaryCondition()
		{
			for (int i = 0; i < mPs.particles.size(); i++)
			{

				bool invFlag = false;

				if (mPs.particles[i].position.x < mPs.lowerBound.x + Lagrangian3dPara::supportRadius)
				{
					mPs.particles[i].velocity.x = abs(mPs.particles[i].velocity.x);
					invFlag = true;
				}

				if (mPs.particles[i].position.y < mPs.lowerBound.y + Lagrangian3dPara::supportRadius)
				{
					mPs.particles[i].velocity.y = abs(mPs.particles[i].velocity.y);
					invFlag = true;
				}

				if (mPs.particles[i].position.z < mPs.lowerBound.z + Lagrangian3dPara::supportRadius)
				{
					mPs.particles[i].velocity.z = abs(mPs.particles[i].velocity.z);
					invFlag = true;
				}

				if (mPs.particles[i].position.x > mPs.upperBound.x - Lagrangian3dPara::supportRadius)
				{
					mPs.particles[i].velocity.x = -abs(mPs.particles[i].velocity.x);
					invFlag = true;
				}

				if (mPs.particles[i].position.y > mPs.upperBound.y - Lagrangian3dPara::supportRadius)
				{
					mPs.particles[i].velocity.y = -abs(mPs.particles[i].velocity.y);
					invFlag = true;
				}

				if (mPs.particles[i].position.z > mPs.upperBound.z - Lagrangian3dPara::supportRadius)
				{
					mPs.particles[i].velocity.z = -abs(mPs.particles[i].velocity.z);
					invFlag = true;
				}

				if (invFlag)
				{
					mPs.particles[i].velocity *= Lagrangian3dPara::velocityAttenuation;
				}

				glm::vec3 newPosition, newVelocity;
				for (int j = 0; j < 3; j++)
				{
					newPosition[j] = max((mPs.lowerBound[j] + Lagrangian3dPara::supportRadius + Lagrangian3dPara::eps), min(mPs.particles[i].position[j], (mPs.upperBound[j] - (Lagrangian3dPara::supportRadius + Lagrangian3dPara::eps))));
					newVelocity[j] = max(-Lagrangian3dPara::maxVelocity, min(mPs.particles[i].velocity[j], Lagrangian3dPara::maxVelocity));
				}
				mPs.particles[i].position = newPosition;
				mPs.particles[i].velocity = newVelocity;
			}
		}

		void Solver::calculateBlockId()
		{
			for (int i = 0; i < mPs.particles.size(); i++)
			{
				glm::vec3 deltePos = mPs.particles[i].position - mPs.lowerBound;
				glm::vec3 blockPosition = glm::floor(deltePos / mPs.blockSize);
				mPs.particles[i].blockId = blockPosition.z * mPs.blockNum.x * mPs.blockNum.y + blockPosition.y * mPs.blockNum.x + blockPosition.x;
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
							glm::vec3 radiusIj = mPs.particles[i].position - mPs.particles[j].position;
							float diatanceIj = length(radiusIj);
							if (diatanceIj <= Lagrangian3dPara::supportRadius)
							{
								mPs.particles[i].density += mW.GetValueAndGrad(diatanceIj / Lagrangian3dPara::supportRadius).r;
							}
						}
					}
				}

				mPs.particles[i].density *= (mPs.particleVolume * Lagrangian3dPara::density);
				mPs.particles[i].density = max(mPs.particles[i].density, Lagrangian3dPara::density);
				mPs.particles[i].pressure = Lagrangian3dPara::stiffness * (pow(mPs.particles[i].density / Lagrangian3dPara::density, Lagrangian3dPara::exponent) - 1.0);
				mPs.particles[i].pressDivDens2 = mPs.particles[i].pressure / pow(mPs.particles[i].density, 2);
			}
		}
	}

}