#include "Configure.h"

int imageWidth = 800;
int imageHeight = 800;

int theDim2d[2] = { 100, 100 };
double theCellSize2d = 0.5;

int theDim3d[3] = { 50, 50, 50 };
double theCellSize3d = 0.5;


namespace SPH2D {
    // 求解器参数
    extern float dt = 4e-4;
    extern int substep = 5;
    extern float exponent = 7.0f;
    // 物理参数
    extern float gravity = 9.8f;
    extern float density = 1000.0f;
    extern float stiffness = 70.0f;
    extern float viscosity = 0.03f;
}

namespace SPH3D {
    // 求解器参数
    extern float dt = 4e-4;
    extern int substep = 5;
    extern float maxVelocity = 100;
    extern float velocityAttenuation = 0.7;
    extern float eps = 1e-5;

    // 物理参数
    extern float supportRadius = 0.025;
    extern float particalRadius = 0.005;
    extern float particalDiameter = particalRadius * 2.0;
    extern float gravity = 9.8f;
    extern float density0 = 1000.0f;
    extern float stiffness = 5.0f;
    extern float exponent = 7.0f;   // 求解方程中的指数项
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


    extern const glm::vec3 vertexes[] = {
            glm::vec3(0.0, 0.0, 0.0),
            glm::vec3(1.0, 0.0, 0.0),
            glm::vec3(0.0, 1.0, 0.0),
            glm::vec3(0.0, 0.0, 1.0)
    };

    extern const GLuint indices[] = {
        0, 1, 0, 2, 0, 3
    };

    extern std::vector<float_t> floorVertices = {
        // vertex           texCoord
         1.0f,  1.0f, 0.0f, 1.0, 1.0,
        -1.0f,  1.0f, 0.0f, 0.0, 1.0,
        -1.0f, -1.0f, 0.0f, 0.0, 0.0,
         1.0f,  1.0f, 0.0f, 1.0, 1.0,
        -1.0f, -1.0f, 0.0f, 0.0, 0.0,
         1.0f, -1.0f, 0.0f, 1.0, 0.0,
    };

}

std::vector<Glb::Component*> methodComponents;

std::string shaderPath = "../../../../code/resources/shaders";