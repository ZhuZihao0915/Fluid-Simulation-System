#pragma once
#ifndef __LAGRANGIAN_2D_SOLVER_H__
#define __LAGRANGIAN_2D_SOLVER_H__

#include "ParticleSystem2d.h"
#include "CubicSpline.h"

#include "Configure.h"

namespace FluidSimulation
{

    namespace Lagrangian2d
    {
        class Solver
        {
        public:
            Solver(ParticleSystem2d &ps);
            ~Solver();

            void solve();

        private:
            void eulerIntegration();
            void computeDensityAndPress();
            void computeAccleration();
            void boundaryCondition();
            void calculateBlockId();

        private:
            ParticleSystem2d &mPs;
            Glb::CubicSpline2d mW;
        };
    }
}

#endif
