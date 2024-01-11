#pragma once
#ifndef __SPH_3D_SOLVER_H__
#define __SPH_3D_SOLVER_H__

#include "ParticalSystem3d.h"
#include "WCubicSpline.h"

#include "Iteration.h"

namespace FluidSimulation {

    namespace SPH3d {
        class Solver {
        public:
            explicit Solver(ParticalSystem3d& ps);
            ~Solver();

            void solve();

        private:

            void EulerIntegration();
            void ComputeDensityAndPress();
            void ComputeAccleration();
            void BoundaryCondition();
            void CalculateBlockId();

        private:
            ParticalSystem3d& mPs;
            Glb::WCubicSpline2d mW;

        };
    }
}


#endif
