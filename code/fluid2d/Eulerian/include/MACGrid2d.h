#pragma once
#ifndef __MACGRID_2D_H__
#define __MACGRID_2D_H__

#include <windows.h>
#include "GL/gl.h"
#include <glm/glm.hpp>
#include "GridData2d.h"

namespace FluidSimulation
{
    namespace Eulerian2d
    {
        using namespace boost::numeric;

        enum RenderMode
        {
            NONE,
            DENSITY,
            TEMPERATURE,
            PRESSURE
        };

        class MACGrid2d
        {
            friend MACGrid2d;

        public:
            MACGrid2d();
            ~MACGrid2d();
            MACGrid2d(const MACGrid2d &orig);
            MACGrid2d &operator=(const MACGrid2d &orig);

            void reset();

            glm::vec4 getRenderColor(int i, int j);
            glm::vec4 getRenderColor(const glm::vec2 &pt);

            // Setup
            void initialize();
            void createSolids();
            void updateSources();

            // Simulation
            glm::vec2 traceBack(const glm::vec2 &pt, double dt);
            glm::vec2 getVelocity(const glm::vec2 &pt);
            double getVelocityX(const glm::vec2 &pt);
            double getVelocityY(const glm::vec2 &pt);
            double getTemperature(const glm::vec2 &pt);
            double getDensity(const glm::vec2 &pt);

            enum Direction
            {
                X,
                Y
            };

            // ��������������ȡ���ĵ�λ���Լ���������
            glm::vec2 getCenter(int i, int j);
            glm::vec2 getLeftLine(int i, int j);
            glm::vec2 getRightLine(int i, int j);
            glm::vec2 getTopLine(int i, int j);
            glm::vec2 getBottomLine(int i, int j);

            void getCell(int index, int &i, int &j);
            int getIndex(int i, int j);
            bool isNeighbor(int i0, int j0, int i1, int j1);
            bool isFace(int i, int j, Direction d);
            int isSolidCell(int i, int j);              // Returns 1 if true, else otherwise
            int isSolidFace(int i, int j, Direction d); // Returns 1 if true, else otherwise
            bool inSolid(const glm::vec2 &pt);
            bool inSolid(const glm::vec2 &pt, int &i, int &j);
            bool intersects(const glm::vec2 &pt, const glm::vec2 &dir, int i, int j, double &time);
            int numSolidCells();

            double getPressureCoeffBetweenCells(int i0, int j0, int i1, int j1);
            double getDivergence(int i, int j);   // At center
            double checkDivergence(int i, int j); // At center
            bool checkDivergence();

            // Compute forces
            double getBoussinesqForce(const glm::vec2 &pt);

            glm::vec2 getVorticityN(int i, int j);
            glm::vec2 getVorticity(int i, int j);
            double getConfinementForce(int i, int j);

            float cellSize;
            int dim[2];

        public:
            Glb::GridData2dX mU;     // x
            Glb::GridData2dY mV;     // y
            Glb::CubicGridData2d mD; // density
            Glb::CubicGridData2d mT; // temperature

            Glb::GridData2d mSolid; // solid
        };

#define FOR_EACH_CELL                                                \
    for (int j = 0; j < Eulerian2dPara::theDim2d[MACGrid2d::Y]; j++) \
        for (int i = 0; i < Eulerian2dPara::theDim2d[MACGrid2d::X]; i++)

#define FOR_EACH_LINE                                                    \
    for (int j = 0; j < Eulerian2dPara::theDim2d[MACGrid2d::Y] + 1; j++) \
        for (int i = 0; i < Eulerian2dPara::theDim2d[MACGrid2d::X] + 1; i++)

    }
}

#endif