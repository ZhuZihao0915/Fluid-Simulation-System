#include "SPH/include/Solver.h"
#include "Global.h"
#include <iostream>
#include <algorithm>

namespace FluidSimulation {

    namespace SPH2d {
        Solver::Solver(ParticalSystem2d& ps) : mPs(ps), mW(ps.supportRadius)
        {

        }

        Solver::~Solver() {

        }

        void Solver::solve() {
            updateDensityAndPressure();
            initAccleration();
            updateViscosityAccleration();
            updatePressureAccleration();
            eulerIntegrate();
            boundaryCondition();
        }

        void Solver::updateDensityAndPressure() {
            // initialized as normal condition
            mPs.mDensity = std::vector<float>(mPs.positions.size(), SPH2D::density);
            mPs.mPressure = std::vector<float>(mPs.positions.size(), 0.0f);

            // caculate
            for (int i = 0; i < mPs.positions.size(); i++) {
                // if have neighbor(s) then we need to update the density
                if (mPs.mNeighbors.size() != 0) {
                    std::vector<Neighbor>& neighbors = mPs.mNeighbors[i];
                    float density = 0;
                    // calculate the contribution of the neighbors to density
                    for (auto& nInfo : neighbors) {
                        density += mW.Value(nInfo.distance);    // kernel function (interpolation)
                    }
                    density *= (mPs.volume * SPH2D::density);
                    mPs.mDensity[i] = density;
                    mPs.mDensity[i] = max(density, SPH2D::density);        // Ω˚÷π≈Ú’Õ
                }
                // pressure
                mPs.mPressure[i] = SPH2D::stiffness * (std::powf(mPs.mDensity[i] / SPH2D::density, SPH2D::exponent) - 1.0f);
            }
        }

        // initialize the accleration as gravity
        void Solver::initAccleration() {
            std::fill(mPs.accleration.begin() + mPs.mStartIndex, mPs.accleration.end(), glm::vec2(0.0f, -SPH2D::gravity));
        }

        // update accleration by viscosity
        void Solver::updateViscosityAccleration() {
            float dim = 2.0f;
            float constFactor = 2.0f * (dim + 2.0f) * SPH2D::viscosity;
            // traverse all particles
            for (int i = mPs.mStartIndex; i < mPs.positions.size(); i++) {
                // if have neighbor(s)
                if (mPs.mNeighbors.size() != 0) {
                    // caculate the viscous force which depend on the differences between two particles' velocity and position
                    std::vector<Neighbor>& neighbors = mPs.mNeighbors[i];
                    glm::vec2 viscosityForce(0.0f, 0.0f);
                    for (auto& nInfo : neighbors) {
                        int j = nInfo.index;
                        float dotDvToRad = glm::dot(mPs.velocity[i] - mPs.velocity[j], nInfo.radius);

                        float denom = nInfo.distance2 + 0.01f * mPs.supportRadius2;
                        // 
                        viscosityForce += (mPs.mass / mPs.mDensity[j]) * dotDvToRad * mW.Grad(nInfo.radius) / denom;
                    }
                    viscosityForce *= constFactor;
                    mPs.accleration[i] += viscosityForce;
                }
            }
        }

        // update accleration by pressure
        void Solver::updatePressureAccleration() {
            // get p/(dens^2)
            std::vector<float> pressDivDens2(mPs.positions.size(), 0);
            for (int i = 0; i < mPs.positions.size(); i++) {
                pressDivDens2[i] = mPs.mPressure[i] / std::powf(mPs.mDensity[i], 2);
            }
            // traverse all particles
            for (int i = mPs.mStartIndex; i < mPs.positions.size(); i++) {
                if (mPs.mNeighbors.size() != 0) {
                    std::vector<Neighbor>& neighbors = mPs.mNeighbors[i];
                    glm::vec2 pressureForce(0.0f, 0.0f);
                    // traverse all neighbors, caculate pressure force
                    for (auto& nInfo : neighbors) {
                        int j = nInfo.index;
                        pressureForce += mPs.mDensity[j] * (pressDivDens2[i] + pressDivDens2[j]) * mW.Grad(nInfo.radius);
                    }
                    mPs.accleration[i] -= pressureForce * mPs.volume;
                }
            }
        }

        // after dt time
        void Solver::eulerIntegrate() {
            // traverse all particles
            for (int i = mPs.mStartIndex; i < mPs.positions.size(); i++) {
                // use accleration update velocity
                mPs.velocity[i] += SPH2D::dt * mPs.accleration[i];
                // restrict
                mPs.velocity[i] = glm::clamp(mPs.velocity[i], glm::vec2(-100.0f), glm::vec2(100.0f));
                // use velocity update position
                mPs.positions[i] += SPH2D::dt * mPs.velocity[i];
            }
        }

        // 
        void Solver::boundaryCondition() {
            // traverse all particles
            for (int i = 0; i < mPs.positions.size(); i++) {
                glm::vec2& position = mPs.positions[i];
                bool invFlag = false;
                // if particles are approaching the boundary, reverse their direction in x(left/right) or y(up/down)
                if (position.y < mPs.lowerLeftCorner.y + mPs.supportRadius) {
                    mPs.velocity[i].y = std::abs(mPs.velocity[i].y);
                    invFlag = true;
                }

                if (position.y > mPs.upperRightCorner.y - mPs.supportRadius) {
                    mPs.velocity[i].y = -std::abs(mPs.velocity[i].y);
                    invFlag = true;
                }

                if (position.x < mPs.lowerLeftCorner.x + mPs.supportRadius) {
                    mPs.velocity[i].x = std::abs(mPs.velocity[i].x);
                    invFlag = true;
                }

                if (position.x > mPs.upperRightCorner.x - mPs.supportRadius) {
                    mPs.velocity[i].x = -std::abs(mPs.velocity[i].x);
                    invFlag = true;
                }
                // if does, update their position by new velocities
                if (invFlag) {
                    mPs.positions[i] += SPH2D::dt * mPs.velocity[i];
                    mPs.velocity[i] = glm::clamp(mPs.velocity[i], glm::vec2(-100.0f), glm::vec2(100.0f)); // restriction
                }
            }
        }
    }
}

