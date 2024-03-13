#pragma once
#ifndef __LAGRANGIAN_3D_SOLVER_H__
#define __LAGRANGIAN_3D_SOLVER_H__

#include "ParticalSystem3d.h"
#include "WCubicSpline.h"
#include "Global.h"
#include "Configure.h"
#include <iostream>

namespace FluidSimulation {

    namespace Lagrangian3d {
        class Solver {
        public:
            Solver(ParticalSystem3d& ps);
            ~Solver();

            void solve();

        private:

            void eulerIntegration();
            void computeDensityAndPress();
            void computeAccleration();
            void boundaryCondition();
            void calculateBlockId();

        private:
            ParticalSystem3d& mPs;
            Glb::WCubicSpline3d mW;

        };
    }
}


#endif
