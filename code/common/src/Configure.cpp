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
    float contrast = 1;     // �����Աȶ�
    int drawModel = 0;
    int gridNum = theDim2d[0];

    // solver settings
    float dt = 0.01;
    float sourceVelocity = 1.0f;    // �ͷų�����ĳ��ٶ�
    float airDensity = 1.3;         // �����ܶ�
    float ambientTemp = 0.0;        // �����¶�
    float boussinesqAlpha = 500.0;  // Boussinesq Force ��ʽ�е� alpha ����
    float boussinesqBeta = 2500.0;  // Boussinesq Force ��ʽ�е� beta ����
    float vorticityConst = 100.0;   // �����ж�Լ������ǿ��
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
    // scale: ��ά������scale��
    // ͨ������scale�Ĵ�С���Ŵ�������������С���Ӷ�����������
    float scale = 2;

    // solver settings
    float dt = 0.0016;
    int substep = 1;
    float maxVelocity = 100;            // ��������ٶ�
    float velocityAttenuation = 0.7;    // �������߽����ٶ�˥��ϵ��
    float eps = 1e-5;                   // һ����С�ľ��룬���ڱ߽��⣬��ֹ������������

    // particle system
    float supportRadius = 0.04;     // �ڴ˰뾶�ڵ��������ӻ�Ը����Ӳ���Ӱ��
    float particleRadius = 0.01;    // ���ӵİ뾶
    float particleDiameter = particleRadius * 2.0;
    float gravityX = 0.0f;          // x���ϵ�����
    float gravityY = 9.8f;          // y���ϵ�����
    float density = 1000.0f;        // �����ܶ�
    float stiffness = 70.0f;        // �ն�
    float exponent = 7.0f;          // ѹ�����㹫ʽ��ָ��
    float viscosity = 0.03f;        // ճ��
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