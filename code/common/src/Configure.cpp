#include "Configure.h"

int imageWidth = 800;
int imageHeight = 800;

bool simulating = false;

namespace Eulerian2dPara
{
    int theDim2d[2] = {100, 100};
    float theCellSize2d = 0.5;

    float dt = 0.01;

    int drawModel = 1;
    int gridNum = theDim2d[0];

    float sourceVelocity = 1.0f;
    float airDensity = 1.0; //(1.3 kg/m^2) in reality,
    float ambientTemp = 0.0;
    float boussinesqAlpha = 500.0;
    float boussinesqBeta = 2500.0;
    float vorticityConst = 100.0;
}

namespace Eulerian3dPara
{
    int theDim3d[3] = {32, 32, 32};
    float theCellSize3d = 0.5;

    // Rendering
    bool oneSheet = true;
    float distanceX = 0.5;
    float distanceY = 0.5;
    float distanceZ = 0.5;
    bool xySheetsON = false;
    bool yzSheetsON = true;
    bool xzSheetsON = false;
    int drawModel = 1;
    int gridNumX = 100;
    int gridNumY = 100;
    int gridNumZ = 100;

    int xySheetsNum = 3;
    int yzSheetsNum = 3;
    int xzSheetsNum = 3;

    float dt = 0.01;

    float sourceVelocity = 1.0f;

    float airDensity = 1.0;
    float ambientTemp = 0.0;
    float boussinesqAlpha = 500.0;
    float boussinesqBeta = 2500.0;
    float vorticityConst = 100.0;

}

namespace Lagrangian2dPara
{

    float dt = 6e-4;
    int substep = 3;
    float maxVelocity = 100;
    float velocityAttenuation = 0.7;
    float eps = 1e-5;


    float supportRadius = 0.04;  // 0.025;
    float particalRadius = 0.01; // 0.005;
    float particalDiameter = particalRadius * 2.0;
    float gravityX = 0.0f;
    float gravityY = 9.8f;
    float density = 1000.0f;
    float stiffness = 70.0f;
    float exponent = 7.0f;
    float viscosity = 0.03f;
}

namespace Lagrangian3dPara
{
    float dt = 0.002;
    int substep = 1;
    float maxVelocity = 100;
    float velocityAttenuation = 0.7;
    float eps = 1e-5;

    float supportRadius = 0.04;  // 0.025;
    float particalRadius = 0.01; // 0.005;
    float particalDiameter = particalRadius * 2.0;

    float gravityX = 0.0f;
    float gravityY = 0.0f;
    float gravityZ = 9.8f;

    float density0 = 1000.0f;
    float stiffness = 20.0f;
    float exponent = 7.0f;
    float viscosity = 8e-5f;

    float IOR = 1.3;
    float IOR_BIAS = 0.02;
    glm::vec3 F0 = {0.15, 0.15, 0.15};
    glm::vec3 FLUID_COLOR = {0.1, 0.5, 1.0};
    glm::vec3 SHADOW_COLOR = 0.5f * FLUID_COLOR;
    float CAUSTIC_FACTOR = 0.004;
    float THICKNESS_FACTOR = 0.5;

}

std::vector<Glb::Component *> methodComponents;

std::string shaderPath = "../../../../code/resources/shaders";
std::string picturePath = "../../../../code/resources/pictures";