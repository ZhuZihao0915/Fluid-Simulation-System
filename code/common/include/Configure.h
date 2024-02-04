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

extern int theDim2d[];
extern double theCellSize2d;

extern int theDim3d[];
extern double theCellSize3d;

namespace SPH2D {
    // ���������
    extern float dt;
    extern int substep;

    // �������
    extern float gravity;
    extern float density;
    extern float stiffness;
    extern float exponent;
    extern float viscosity;
}

namespace SPH3D {
    // ���������
    extern float dt;
    extern int substep;
    extern float maxVelocity;
    extern float velocityAttenuation;
    extern float eps;

    // �������
    extern float supportRadius;
    extern float particalRadius;
    extern float particalDiameter;
    extern float gravity;
    extern float density0;
    extern float stiffness;
    extern float exponent;
    extern float viscosity;

    // ��ѧ����
    extern float IOR;
    extern float IOR_BIAS;
    extern glm::vec3 F0;
    extern glm::vec3 FLUID_COLOR;
    extern glm::vec3 SHADOW_COLOR;
    extern float CAUSTIC_FACTOR;
    extern float THICKNESS_FACTOR;

    // ���β���
    extern float zFar;
    extern float zNear;

    // ��Ⱦ
    extern const glm::vec3 vertexes[];
    extern const GLuint indices[];
    extern std::vector<float_t> floorVertices;
}

extern std::string shaderPath;

extern std::vector<Glb::Component*> methodComponents;

#endif // !__CONFIGURE_H__
