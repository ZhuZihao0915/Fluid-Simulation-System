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

            // �ܶ�Դ�ͷ�����
            mGrid->updateSources();

            // advect �ٶ�
            mGrid->advectVelocity(dt);
            // ���Ǹ������жȵ�����
            mGrid->addExternalForces(dt);
            // project ȷ��incompressible������
            mGrid->project(dt);

            // advect �¶�
            mGrid->advectTemperature(dt);

            // advect �ܶ�
            mGrid->advectDensity(dt);
        }
    }
}
