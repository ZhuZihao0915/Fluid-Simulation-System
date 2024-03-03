#pragma once
#ifndef __CONFIGURE_H__
#define __CONFIGURE_H__

#include <iostream>
#include <string>
#include "Component.h"
#include <vector>
#include "glm/glm.hpp"

#define LERP(a,b,t) (1-t)*a + t*b

#ifndef __MINMAX_DEFINED
#  define max(a,b)    (((a) > (b)) ? (a) : (b))
#  define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

extern int imageWidth;
extern int imageHeight;

extern bool simulating;

namespace MAC2dPara {
    extern int theDim2d[];
    extern float theCellSize2d;

    extern float dt;
    
    extern float sourceVelocity;

    extern float airDensity;
    extern float ambientTemp;
    extern float boussinesqAlpha;
    extern float boussinesqBeta;
    extern float vorticityConst;
}

namespace MAC3dPara{
    extern int theDim3d[];
    extern float theCellSize3d;

    extern bool oneSheet;
    extern float distance;

    extern bool xySheetsON;
    extern bool yzSheetsON;
    extern bool xzSheetsON;

    extern int xySheetsNum;
    extern int yzSheetsNum;
    extern int xzSheetsNum;

    extern float dt;

    extern float sourceVelocity;

    extern float airDensity;
    extern float ambientTemp;
    extern float boussinesqAlpha;
    extern float boussinesqBeta;
    extern float vorticityConst;
    
}

namespace SPH2dPara {
    // 求解器参数
    extern float dt;
    extern int substep;
    extern float maxVelocity;
    extern float velocityAttenuation;
    extern float eps;

    // 物理参数
    extern float supportRadius;
    extern float particalRadius;
    extern float particalDiameter;
    extern float gravity;
    extern float density;
    extern float stiffness;
    extern float exponent;
    extern float viscosity;
}

//namespace SPH2dPara {
//    // 求解器参数
//    extern float dt;
//    extern int substep;
//
//    // 物理参数
//    extern float gravity;
//    extern float density;
//    extern float stiffness;
//    extern float exponent;
//    extern float viscosity;
//}

namespace SPH3dPara {
    // 求解器参数
    extern float dt;
    extern int substep;
    extern float maxVelocity;
    extern float velocityAttenuation;
    extern float eps;

    // 物理参数
    extern float supportRadius;
    extern float particalRadius;
    extern float particalDiameter;
    extern float gravity;
    extern float density0;
    extern float stiffness;
    extern float exponent;
    extern float viscosity;

    // 光学参数
    extern float IOR;
    extern float IOR_BIAS;
    extern glm::vec3 F0;
    extern glm::vec3 FLUID_COLOR;
    extern glm::vec3 SHADOW_COLOR;
    extern float CAUSTIC_FACTOR;
    extern float THICKNESS_FACTOR;

    // 几何参数
    extern float zFar;
    extern float zNear;

    // 渲染
    extern const glm::vec3 vertexes[];
    extern const GLuint indices[];
    extern std::vector<float_t> floorVertices;
}

extern std::string shaderPath;

extern std::vector<Glb::Component*> methodComponents;

#endif // !__CONFIGURE_H__
