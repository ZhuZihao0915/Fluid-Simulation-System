#include "MAC/include/Solver.h"
#include <GL/glut.h>

#include "Configure.h"

namespace FluidSimulation {
    namespace MAC2d {
        Solver::Solver(MACGrid2d* grid)
        {
            this->mGrid = grid;
            reset();
        }

        Solver::~Solver()
        {
        }

        void Solver::reset()
        {
            mGrid->reset();
        }

        void Solver::setGridDimensions(int x, int y)
        {
            theDim2d[0] = x;
            theDim2d[1] = y;
            reset();
        }

        void Solver::solve()
        {
            double dt = 0.01; 

            // 密度源释放流体
            mGrid->updateSources();

            // advect 速度
            mGrid->advectVelocity(dt);
            // 考虑浮力、涡度等因素
            mGrid->addExternalForces(dt);
            // project 确保incompressible的特性
            mGrid->project(dt);

            // advect 温度
            mGrid->advectTemperature(dt);

            // advect 密度
            mGrid->advectDensity(dt);
        }
    }
}
