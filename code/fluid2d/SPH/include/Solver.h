#pragma once
#ifndef __SPH_2D_SOLVER_H__
#define __SPH_2D_SOLVER_H__

#include "ParticalSystem2d.h"
#include "WCubicSpline.h"

#include "Iteration.h"

namespace FluidSimulation {

    namespace SPH2d {
        class Solver {
        public:
            explicit Solver(ParticalSystem2d& ps);
            ~Solver();

            void solve();

        private:
            void updateDensityAndPressure();

            void initAccleration();

            void updateViscosityAccleration();

            void updatePressureAccleration();

            void eulerIntegrate();

            void boundaryCondition();

        private:
            ParticalSystem2d& mPs;
            Glb::WCubicSpline2d mW;

        };
    }
}


#endif
