#pragma once
#ifndef __MAC3D_MACGRID_3D_H__
#define __MAC3D_MACGRID_3D_H__



#include <windows.h>
#include "GL/gl.h"
#include <glm/glm.hpp>
#include "GridData3d.h"

namespace FluidSimulation {
    namespace MAC3d {
        using namespace boost::numeric;

        enum RenderMode { NONE, DENSITY, TEMPERATURE, PRESSURE };

        class MACGrid3d
        {
            friend MACGrid3d;
        public:
            MACGrid3d();
            ~MACGrid3d();
            MACGrid3d(const MACGrid3d& orig);
            MACGrid3d& operator=(const MACGrid3d& orig);

            void reset();

            glm::vec4 getRenderColor(int i, int j, int k);
            glm::vec4 getRenderColor(const glm::vec3& pt);

            // setup
            void initialize();
            void createSolids();

            // 根据位置获得该点的属性值
            glm::vec3 traceBack(const glm::vec3& pt, double dt);
            glm::vec3 getVelocity(const glm::vec3& pt);
            double getVelocityX(const glm::vec3& pt);
            double getVelocityY(const glm::vec3& pt);
            double getVelocityZ(const glm::vec3& pt);
            double getTemperature(const glm::vec3& pt);
            double getDensity(const glm::vec3& pt);

            enum Direction { X, Y, Z };
            
            // 根据网格索引获取中心点位置以及各边向量
            glm::vec3 getCenter(int i, int j, int k);
            glm::vec3 getLeftFace(int i, int j, int k);
            glm::vec3 getRightFace(int i, int j, int k);
            glm::vec3 getTopFace(int i, int j, int k);
            glm::vec3 getBottomFace(int i, int j, int k);
            glm::vec3 getFrontFace(int i, int j, int k);
            glm::vec3 getBackFace(int i, int j, int k);

            void getCell(int index, int& i, int& j, int& k);
            int getIndex(int i, int j, int k);
            bool isNeighbor(int i0, int j0, int k0, int i1, int j1, int k1);
            bool isFace(int i, int j, int k, Direction d);
            int isSolidCell(int i, int j, int k); // Returns 1 if true, else otherwise
            int isSolidFace(int i, int j, int k, Direction d); // Returns 1 if true, else otherwise
            bool inSolid(const glm::vec3& pt);
            bool inSolid(const glm::vec3& pt, int& i, int& j, int& k);
            bool intersects(const glm::vec3& pt, const glm::vec3& dir, int i, int j, int k, double& time);
            int numSolidCells();

            double getPressureCoeffBetweenCells(int i0, int j0, int k0, int i1, int j1, int k1);
            double getDivergence(int i, int j, int k);  // At center
            double checkDivergence(int i, int j, int k);  // At center
            bool checkDivergence();

            double getBoussinesqForce(const glm::vec3& pt);
            glm::vec3 getVorticityN(int i, int j, int k);
            glm::vec3 getVorticity(int i, int j, int k);
            glm::vec3 getConfinementForce(int i, int j, int k);


        public:
            Glb::GridData3dX mU;        // x
            Glb::GridData3dY mV;        // y
            Glb::GridData3dZ mW;        // z
            Glb::CubicGridData3d mD;    // density
            Glb::CubicGridData3d mT;    // temperature
            Glb::GridData3d mSolid;     // solid
        };

    }
}


#endif