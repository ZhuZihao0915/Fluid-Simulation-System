#include "Configure.h"

// TODO(optional)
// add yor parameters

int imageWidth = 800;
int imageHeight = 800;

// simulating or stopped
bool simulating = false;

namespace Eulerian2dPara
{
    // MAC grid
    int theDim2d[2] = {100, 100};
    float theCellSize2d = 0.5;

    // renderer settings
    float contrast = 1;     // 调整对比度
    int drawModel = 0;
    int gridNum = theDim2d[0];

    // solver settings
    float dt = 0.01;
    float sourceVelocity = 1.0f;    // 释放出烟雾的初速度
    float airDensity = 1.3;         // 空气密度
    float ambientTemp = 0.0;        // 环境温度
    float boussinesqAlpha = 500.0;  // Boussinesq Force 公式中的 alpha 参数
    float boussinesqBeta = 2500.0;  // Boussinesq Force 公式中的 beta 参数
    float vorticityConst = 100.0;   // 调整涡度约束力的强度
}

namespace Eulerian3dPara
{
    // MAC grid
    int theDim3d[3] = {12, 36, 36};
    float theCellSize3d = 0.5;

    // renderer settings
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

    // solver settings
    float dt = 0.01;
    float sourceVelocity = 1.0f;
    float airDensity = 1.3;
    float ambientTemp = 0.0;
    float boussinesqAlpha = 500.0;
    float boussinesqBeta = 2500.0;
    float vorticityConst = 100.0;

}

namespace Lagrangian2dPara
{
    // scale: 各维度扩大scale倍
    // 通过调整scale的大小，放大容器与流体块大小，从而增加粒子数
    float scale = 2;

    // solver settings
    float dt = 0.0016;
    int substep = 1;
    float maxVelocity = 100;            // 粒子最大速度
    float velocityAttenuation = 0.7;    // 碰到到边界后的速度衰减系数
    float eps = 1e-5;                   // 一个极小的距离，用于边界检测，防止粒子脱离容器

    // particle system
    float supportRadius = 0.04;     // 在此半径内的其他粒子会对该粒子产生影响
    float particleRadius = 0.01;    // 粒子的半径
    float particleDiameter = particleRadius * 2.0;
    float gravityX = 0.0f;          // x轴上的重力
    float gravityY = 9.8f;          // y轴上的重力
    float density = 1000.0f;        // 粒子密度
    float stiffness = 70.0f;        // 刚度
    float exponent = 7.0f;          // 压力计算公式的指数
    float viscosity = 0.03f;        // 粘度
}

namespace Lagrangian3dPara
{
    // scale
    float scale = 1.2;
    
    // solver settings
    float dt = 0.002;
    int substep = 1;
    float maxVelocity = 100;
    float velocityAttenuation = 0.7;
    float eps = 1e-5;

    // particle system
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

// stores system's all simulation method components
std::vector<Glb::Component *> methodComponents;

std::string shaderPath = "../../../../code/resources/shaders";
std::string picturePath = "../../../../code/resources/pictures";