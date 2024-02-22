#pragma once
#ifndef __MAC2D_MACGRID_2D_H__
#define __MAC2D_MACGRID_2D_H__

#include <windows.h>
#include "GL/gl.h"
#include <glm/glm.hpp>
#include "GridData2d.h"

namespace FluidSimulation {
    namespace MAC2d {
        using namespace boost::numeric;

        enum RenderMode { NONE, DENSITY, TEMPERATURE, PRESSURE };

        class MACGrid2d
        {
            friend MACGrid2d;
        public:
            MACGrid2d();
            ~MACGrid2d();
            MACGrid2d(const MACGrid2d& orig);
            MACGrid2d& operator=(const MACGrid2d& orig);

            void reset();

            void draw();
            void updateSources();
            void advectVelocity(double dt);
            void addExternalForces(double dt);
            void project(double dt);
            void advectTemperature(double dt);
            void advectDensity(double dt);

            glm::vec4 getRenderColor(int i, int j);
            glm::vec4 getRenderColor(const glm::vec2& pt);

            // Setup
            void initialize();
            void createSolids();

            // Rendering
            struct Cube { glm::vec2 pos; glm::vec4 color; double dist; };
            void drawWireGrid();
            void drawSolids();
            void drawSmokeCubes();
            void drawSmoke();
            void drawCube(const MACGrid2d::Cube& c);
            void drawFace(const MACGrid2d::Cube& c);
            void drawVelocities();
            void drawVForces();

            void drawZSheets(bool backToFront);
            void drawXSheets(bool backToFront);

            // Simulation
            glm::vec2 traceBack(const glm::vec2& pt, double dt);
            glm::vec2 getVelocity(const glm::vec2& pt);
            double getVelocityX(const glm::vec2& pt);
            double getVelocityY(const glm::vec2& pt);
            
            double getTemperature(const glm::vec2& pt);
            double getDensity(const glm::vec2& pt);

            enum Direction { X, Y };
            
            // 根据网格索引获取中心点位置以及各边向量
            glm::vec2 getCenter(int i, int j);
            glm::vec2 getLeftLine(int i, int j);
            glm::vec2 getRightLine(int i, int j);
            glm::vec2 getTopLine(int i, int j);
            glm::vec2 getBottomLine(int i, int j);

            void getCell(int index, int& i, int& j);
            int getIndex(int i, int j);
            bool isNeighbor(int i0, int j0, int i1, int j1);
            bool isFace(int i, int j, Direction d);
            int isSolidCell(int i, int j); // Returns 1 if true, else otherwise
            int isSolidFace(int i, int j, Direction d); // Returns 1 if true, else otherwise
            bool inSolid(const glm::vec2& pt);
            bool inSolid(const glm::vec2& pt, int& i, int& j);
            bool intersects(const glm::vec2& pt, const glm::vec2& dir, int i, int j, double& time);
            int numSolidCells();

            double getPressureCoeffBetweenCells(
                int i0, int j0, int i1, int j1);
            double getDivergence(int i, int j);  // At center
            double checkDivergence(int i, int j);  // At center
            bool checkDivergence();

            // Compute forces
            double getBoussinesqForce(const glm::vec2& pt);

            glm::vec2 getVorticityN(int i, int j);
            glm::vec2 getVorticity(int i, int j);
            double getConfinementForce(int i, int j);


        public:

            Glb::GridData2dX mU;
            Glb::GridData2dY mV;
            Glb::CubicGridData2d mD;
            Glb::CubicGridData2d mT;

            Glb::GridData2d mSolid;

        };

    }
}


#endif