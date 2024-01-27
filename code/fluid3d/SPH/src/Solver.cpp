#include "fluid3d/SPH/include/Solver.h"

namespace FluidSimulation {

	namespace SPH3d {
		Solver::Solver(ParticalSystem3d& ps) : mPs(ps), mW(ps.mSupportRadius)
		{

		}

		Solver::~Solver() {

		}

		void Solver::solve() {
			computeAccleration();
			eulerIntegration();
			boundaryCondition();
			calculateBlockId();
			computeDensityAndPress();
		}

		void Solver::computeAccleration() {
			for (int i = 0; i < mPs.mParticalInfos.size(); i++) {

				float dim = 3.0;
				float constFactor = 2.0 * (dim + 2.0) * mPs.mViscosity;
				glm::vec3 viscosityForce = glm::vec3(0.0);
				glm::vec3 pressureForce = glm::vec3(0.0);
				for (int i = 0; i < mPs.mBlockIdOffs.size(); i++) {   
					int bIdj = mPs.mParticalInfos[i].blockId + mPs.mBlockIdOffs[i];
					for (int j = mPs.mBlockExtens[bIdj].x; j < mPs.mBlockExtens[bIdj].y; j++) {
						glm::vec3 radiusIj = mPs.mParticalInfos[i].position - mPs.mParticalInfos[j].position;
						float diatanceIj = length(radiusIj);
						if (diatanceIj <= SPH3D::supportRadius) {
							float dotDvToRad = glm::dot(mPs.mParticalInfos[i].velocity - mPs.mParticalInfos[j].velocity, radiusIj);
							float denom = diatanceIj * diatanceIj + 0.01 * SPH3D::supportRadius * SPH3D::supportRadius;
							glm::vec3 wGrad = mW.GetGrad(diatanceIj / SPH3D::supportRadius)[2] * radiusIj;
							viscosityForce += (float)(0.5 / mPs.mParticalInfos[j].density) * dotDvToRad * wGrad / denom;
							pressureForce += mPs.mParticalInfos[j].density * (mPs.mParticalInfos[i].pressDivDens2 + mPs.mParticalInfos[j].pressDivDens2) * wGrad;
						}
					}
				}

				mPs.mParticalInfos[i].accleration += viscosityForce * constFactor;
				mPs.mParticalInfos[i].accleration -= pressureForce * mPs.mVolume;

			}
		}

		void Solver::eulerIntegration() {

		}

		void Solver::computeDensityAndPress() {

		}


		void Solver::boundaryCondition() {

		}

		void Solver::calculateBlockId() {

		}

	}

}