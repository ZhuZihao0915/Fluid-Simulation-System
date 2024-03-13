#pragma once
#ifndef __LAGRANGIAN_2D_SOLVER_H__
#define __LAGRANGIAN_2D_SOLVER_H__

#include "ParticalSystem2d.h"
#include "WCubicSpline.h"

#include "Configure.h"

namespace FluidSimulation {

    namespace Lagrangian2d {
        class Solver {
        public:
            Solver(ParticalSystem2d& ps);
            ~Solver();

            void solve();

        private:
            void eulerIntegration();
            void computeDensityAndPress();
            void computeAccleration();
            void boundaryCondition();
            void calculateBlockId();

        private:
            ParticalSystem2d& mPs;
            Glb::WCubicSpline2d mW;

        };
    }
}


#endif
