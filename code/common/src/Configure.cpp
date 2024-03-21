#include "Configure.h"

// TODO(optional)
// add yor parameters

int imageWidth = 800;
int imageHeight = 800;

// simulating or stopped
bool simulating = false;

namespace Eulerian2dPara
{
    int theDim2d[2] = {100, 100};
    float theCellSize2d = 0.5;

    float dt = 0.01;

    float contrast = 1;
    int drawModel = 0;
    int gridNum = theDim2d[0];

    float sourceVelocity = 1.0f;
    float airDensity = 1.0;     //(1.3 kg/m^2) in reality
    float ambientTemp = 0.0;
    float boussinesqAlpha = 500.0;
    float boussinesqBeta = 2500.0;
    float vorticityConst = 100.0;
}

namespace Eulerian3dPara
{
    // grid set
    int theDim3d[3] = {12, 36, 36};
    float theCellSize3d = 0.5;

    // renderer set
    float contrast = 1;
    bool oneSheet = true;
    float distanceX = 0.52;
    float distanceY = 0.52;
    float distanceZ = 0.52;
    bool xySheetsON = false;
    bool yzSheetsON = true;
    bool xzSheetsON = false;
    int drawModel = 1;
    int gridNumX = (int)((float)theDim3d[0] / theDim3d[2] * 100);
    int gridNumY = (int)((float)theDim3d[1] / theDim3d[2] * 100);
    int gridNumZ = 100;

    int xySheetsNum = 3;
    int yzSheetsNum = 3;
    int xzSheetsNum = 3;

    // solver set
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
    float scale = 2;
    float dt = 0.0016;
    int substep = 1;
    float maxVelocity = 100;
    float velocityAttenuation = 0.7;
    float eps = 1e-5;

    float supportRadius = 0.04;  // 0.025;
    float particleRadius = 0.01; // 0.005;
    float particleDiameter = particleRadius * 2.0;
    float gravityX = 0.0f;
    float gravityY = 9.8f;
    float density = 1000.0f;
    float stiffness = 70.0f;
    float exponent = 7.0f;
    float viscosity = 0.03f;
}

namespace Lagrangian3dPara
{
    float scale = 1;
    float dt = 0.002;
    int substep = 1;
    float maxVelocity = 100;
    float velocityAttenuation = 0.7;
    float eps = 1e-5;

    float supportRadius = 0.04;
    float particleRadius = 0.01;
    float particleDiameter = particleRadius * 2.0;

    float gravityX = 0.0f;
    float gravityY = 0.0f;
    float gravityZ = 9.8f;

    float density = 1000.0f;
    float stiffness = 20.0f;
    float exponent = 7.0f;
    float viscosity = 8e-5f;

}

// store system's all simulation method components
std::vector<Glb::Component *> methodComponents;

std::string shaderPath = "../../../../code/resources/shaders";
std::string picturePath = "../../../../code/resources/pictures";