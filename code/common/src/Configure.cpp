#include "Configure.h"

int imageWidth = 600;
int imageHeight = 600;

int theDim2d[2] = { 100, 100 };
double theCellSize2d = 0.5;

int theDim3d[3] = { 50, 50, 50 };
double theCellSize3d = 0.5;

namespace SPH3D {
    // 求解器参数
    extern float dt = 2e-4;
    extern int substep = 8;

    // 物理参数
    extern float supportRadius = 0.025;
    extern float particalRadius = 0.005;
    extern float particalDiameter = particalRadius * 2.0;
    extern float gravity = 9.8f;
    extern float density0 = 1000.0f;
    extern float stiffness = 5.0f;
    extern float exponent = 7.0f;
    extern float viscosity = 8e-6f;

    // 光学参数
    extern float IOR = 1.3;
    extern float IOR_BIAS = 0.02;
    extern glm::vec3 F0 = { 0.15, 0.15, 0.15 };
    extern glm::vec3 FLUID_COLOR = { 0.1, 0.5, 1.0 };
    extern glm::vec3 SHADOW_COLOR = 0.5f * FLUID_COLOR;
    extern float CAUSTIC_FACTOR = 0.004;
    extern float THICKNESS_FACTOR = 0.5;

    // 几何参数
    extern float zFar = 100.0;
    extern float zNear = 0.1;
}

std::vector<Glb::Component*> methodComponents;

std::string shaderPath = "D:/CG/Fluid-Simulation-System/code/resources/shaders";