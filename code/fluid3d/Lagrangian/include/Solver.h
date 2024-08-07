#pragma once
#ifndef __LAGRANGIAN_3D_SOLVER_H__
#define __LAGRANGIAN_3D_SOLVER_H__

#include "ParticleSystem3d.h"
#include "CubicSpline.h"
#include "Global.h"
#include "Configure.h"
#include <iostream>

namespace FluidSimulation
{

    namespace Lagrangian3d
    {
        class Solver
        {
        public:
            Solver(ParticleSystem3d &ps);

            void solve();

        private:
            void eulerIntegration();
            void computeDensityAndPress();
            void computeAccleration();
            void boundaryCondition();
            void calculateBlockId();

        private:
            ParticleSystem3d &mPs;
            Glb::CubicSpline3d mW;
        };
    }
}

#endif
