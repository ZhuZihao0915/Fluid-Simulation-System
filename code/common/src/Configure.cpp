#include "Configure.h"

int imageWidth = 800;
int imageHeight = 800;

bool simulating = false;

namespace MAC2dPara {
    extern int theDim2d[2] = { 100, 100 };
    extern float theCellSize2d = 0.5;

    extern float dt = 0.01;

    extern float sourceVelocity = 1.0f;

    extern float airDensity = 1.0; //(1.3 kg/m^2) in reality,
    extern float ambientTemp = 0.0;
    extern float boussinesqAlpha = 500.0;
    extern float boussinesqBeta = 2500.0;
    extern float vorticityConst = 100.0;
}

namespace MAC3dPara {
    extern int theDim3d[3] = { 20, 20, 20 };
    extern float theCellSize3d = 0.5;


    // Rendering
    extern bool oneSheet = true;
    extern float distance = 0.5;
    extern bool xySheetsON = false;
    extern bool yzSheetsON = true;
    extern bool xzSheetsON = false;

    extern int xySheetsNum = 5;
    extern int yzSheetsNum = 5;
    extern int xzSheetsNum = 5;

    extern float dt = 0.01;

    extern float sourceVelocity=1.0f;

    extern float airDensity = 1.0; //(1.3 kg/m^2) in reality,
    extern float ambientTemp = 0.0;
    extern float boussinesqAlpha = 500.0;
    extern float boussinesqBeta = 2500.0;
    extern float vorticityConst = 100.0;

}



namespace SPH2dPara {
    // ���������
    extern float dt = 6e-4;
    extern int substep = 3;
    extern float maxVelocity = 100;
    extern float velocityAttenuation = 0.7;
    extern float eps = 1e-5;

    // �������
    extern float supportRadius = 0.04;// 0.025;
    extern float particalRadius = 0.01;// 0.005;
    extern float particalDiameter = particalRadius * 2.0;
    extern float gravity = 9.8f;
    extern float density = 1000.0f;
    extern float stiffness = 70.0f;
    extern float exponent = 7.0f;   // ��ⷽ���е�ָ����
    extern float viscosity = 0.03f;
}

//namespace SPH2dPara {
//    // ���������
//    extern float dt = 4e-4;
//    extern int substep = 5;
//    extern float exponent = 7.0f;
//    // �������
//    extern float gravity = 9.8f;
//    extern float density = 1000.0f;
//    extern float stiffness = 70.0f;
//    extern float viscosity = 0.03f;
//}

namespace SPH3dPara {
    // ���������
    extern float dt = 0.002;
    extern int substep = 1;
    extern float maxVelocity = 100;
    extern float velocityAttenuation = 0.7;
    extern float eps = 1e-5;

    // �������
    extern float supportRadius = 0.04;// 0.025;
    extern float particalRadius = 0.01;// 0.005;
    extern float particalDiameter = particalRadius * 2.0;
    extern float gravity = 9.8f;
    extern float density0 = 1000.0f;
    extern float stiffness = 20.0f;
    extern float exponent = 7.0f;   // ��ⷽ���е�ָ����
    extern float viscosity = 8e-5f;

    // ��ѧ����
    extern float IOR = 1.3;
    extern float IOR_BIAS = 0.02;
    extern glm::vec3 F0 = { 0.15, 0.15, 0.15 };
    extern glm::vec3 FLUID_COLOR = { 0.1, 0.5, 1.0 };
    extern glm::vec3 SHADOW_COLOR = 0.5f * FLUID_COLOR;
    extern float CAUSTIC_FACTOR = 0.004;
    extern float THICKNESS_FACTOR = 0.5;

    // ���β���
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