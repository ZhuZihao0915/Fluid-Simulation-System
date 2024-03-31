# 代码说明

# 文件目录

```bash
---- code
  |- common         # 通用工具，Camera、Component、Shader等
  |- fluid2d        # 二维流体模拟
     |- Eulerian        # 欧拉描述
     |- Lagrangian      # 拉格朗日描述
  |- fluid3d        # 三维流体模拟
     |- Eulerian        # 欧拉描述
     |- Lagrangian      # 拉格朗日描述
  |- third_party    # 第三方库, boost, glfw, glm, imgui, stb_image, glad等
  |- ui        # imgui搭建的ui系统
```

# 参数说明

系统中的重要参数均在`./code/common/src/Configure.cpp`中定义。其中包括流体求解中用到的各种物理参数，可以在文件中更改默认值，也可以通过 UI 中提供的组件在运行中更改。

# 模拟流程

每一个模拟方法都有 4 个关键类

1. Particle System / MAC grid: 用来描述流体
2. Renderer: 根据粒子的位置、或网格的密度来渲染流体
3. Solver: 求解 dt 时间内，流体的变化
4. Component: 组织上述 3 者，完成模拟任务

下面我们以二维场景下拉格朗日描述的流体模拟为例，讲解模拟过程：

1. 当我们选择一个模拟方法后，将调用`Component::init()`函数进行初始化。

```cpp
void Lagrangian2dComponent::init()
{
    if (renderer != NULL || solver != NULL || ps != NULL)
    {
        shutDown();
    }

    Glb::Timer::getInstance().clear();

    // initialize renderer
    renderer = new Renderer();
    renderer->init();

    // initialize particle system
    // set the container's size
    ps = new ParticleSystem2d();
    ps->setContainerSize(glm::vec2(-1.0f, -1.0f), glm::vec2(2.0f, 2.0f));

    // add a fluid block
    ps->addFluidBlock(glm::vec2(-0.4, -0.4), glm::vec2(0.8, 0.8), glm::vec2(-0.0f, -0.0f), 0.02f);

    ps->updateBlockInfo();

    Glb::Logger::getInstance().addLog("2d Particle System initialized. particle num: " + std::to_string(ps->particles.size()));

    solver = new Solver(*ps);
}
```

上述代码初始化了渲染器，设置了粒子系统的容器大小，并添加了流体块。最后将粒子系统传递给求解器。

2. 每当系统需要渲染出新的一帧，它都会调用`Component::simulate()`函数。

```cpp
void Lagrangian2dComponent::simulate()
{
    for (int i = 0; i < Lagrangian2dPara::substep; i++)
    {
        ps->updateBlockInfo();
        solver->solve();
    }
}
```

粒子系统更新了 Block 信息（用于临近搜索加速），求解器进行求解。

3. 求解完成后，ui 通过`Component::getRenderedTexture()`获得渲染结果。

```cpp
GLuint Lagrangian2dComponent::getRenderedTexture()
{
    if (simulating)
    {
        Glb::Timer::getInstance().start();
    }
    renderer->draw(*ps);
    if (simulating)
    {
        Glb::Timer::getInstance().recordTime("rendering");
    }
    return renderer->getRenderedTexture();
}
```

# 粒子系统 & 网格系统（重要）

求解器的任务就是正确地更新流体的属性，因此同学们应该重点了解该部分内容。

拉格朗日描述下，我们用大量的粒子来形容流体。

```cpp
struct ParticleInfo2d
{
    alignas(8) glm::vec2 position;
    alignas(8) glm::vec2 velocity;
    alignas(8) glm::vec2 accleration;
    alignas(4) float density;
    alignas(4) float pressure;
    alignas(4) float pressDivDens2;
    alignas(4) uint32_t blockId;
};

class ParticleSystem2d
{
public:
    ParticleSystem2d();
    ~ParticleSystem2d();

    void setContainerSize(glm::vec2 containerCorner, glm::vec2 containerSize);
    int32_t addFluidBlock(glm::vec2 corner, glm::vec2 size, glm::vec2 v0, float particleSpace);
    uint32_t getBlockIdByPosition(glm::vec2 position);
    void updateBlockInfo();

public:
    // 粒子参数
    float supportRadius = Lagrangian2dPara::supportRadius;
    float supportRadius2 = supportRadius * supportRadius;
    float particleRadius = Lagrangian2dPara::particleRadius;
    float particleDiameter = Lagrangian2dPara::particleDiameter;
    float particleVolume = particleDiameter * particleDiameter;

    // 存储全部粒子信息
    std::vector<ParticleInfo2d> particles;

    // 容器参数
    glm::vec2 lowerBound = glm::vec2(FLT_MAX);
    glm::vec2 upperBound = glm::vec2(-FLT_MAX);
    glm::vec2 containerCenter = glm::vec2(0.0f);

    // Block结构（加速临近搜索）
    glm::uvec2 blockNum = glm::uvec2(0);
    glm::vec2 blockSize = glm::vec2(0.0f);
    std::vector<glm::uvec2> blockExtens;
    std::vector<int32_t> blockIdOffs;
};
```

欧拉描述下，我们将空间划分为网格。

```cpp
class MACGrid2d
{
public:
    MACGrid2d();
    ~MACGrid2d();
    MACGrid2d(const MACGrid2d &orig);
    MACGrid2d &operator=(const MACGrid2d &orig);

    void reset();

    glm::vec4 getRenderColor(int i, int j);
    glm::vec4 getRenderColor(const glm::vec2 &pt);

    // Setup
    void initialize();
    void createSolids();
    void updateSources();

    // advect
    glm::vec2 traceBack(const glm::vec2 &pt, double dt);

    // get value
    glm::vec2 getVelocity(const glm::vec2 &pt);
    double getVelocityX(const glm::vec2 &pt);
    double getVelocityY(const glm::vec2 &pt);
    double getTemperature(const glm::vec2 &pt);
    double getDensity(const glm::vec2 &pt);

    enum Direction
    {
        X,
        Y
    };

    // get point
    glm::vec2 getCenter(int i, int j);
    glm::vec2 getLeftLine(int i, int j);
    glm::vec2 getRightLine(int i, int j);
    glm::vec2 getTopLine(int i, int j);
    glm::vec2 getBottomLine(int i, int j);

    void getCell(int index, int &i, int &j);
    int getIndex(int i, int j);
    bool isNeighbor(int i0, int j0, int i1, int j1);
    bool isFace(int i, int j, Direction d);

    int isSolidCell(int i, int j);              // Returns 1 if true, else otherwise
    int isSolidFace(int i, int j, Direction d); // Returns 1 if true, else otherwise

    bool inSolid(const glm::vec2 &pt);
    bool inSolid(const glm::vec2 &pt, int &i, int &j);

    bool intersects(const glm::vec2 &pt, const glm::vec2 &dir, int i, int j, double &time);
    int numSolidCells();

    // 计算两个网格单元之间的压力系数
    double getPressureCoeffBetweenCells(int i0, int j0, int i1, int j1);

    // 计算散度
    double getDivergence(int i, int j);
    // 检查散度
    double checkDivergence(int i, int j);
    bool checkDivergence();

    // 计算Boussinesq Force
    double getBoussinesqForce(const glm::vec2 &pt);

    // 计算涡度约束力
    glm::vec2 getConfinementForce(int i, int j);
    glm::vec2 getVorticityN(int i, int j);
    glm::vec2 getVorticity(int i, int j);

    float cellSize;
    int dim[2];

    Glb::GridData2dX mU;        // x velocity
    Glb::GridData2dY mV;        // y velocity
    Glb::CubicGridData2d mD;    // density
    Glb::CubicGridData2d mT;    // temperature

    Glb::GridData2d mSolid;     // solid
};
```

# 求解器的实现

拉格朗日描述下，同学们在`Solver::solve()`中需要实现的可能有：

1. 计算密度与压强
2. 计算加速度
3. 更新速度与位置
4. 检查边界情况
5. 更新 Block 信息
6. ...

欧拉描述下，同学们在`Solver::solve()`中需要实现的可能有：

1. 对流（advect）
2. 计算外部力
3. 投影（project）
4. ...

# 计时（可选）

通过以下形式，将需要计时的阶段放在`recordTime("name")`之前。系统会统计各个阶段占所有计时的百分比并输出。

```cpp
void Solver::solve(){

    Glb::Timer::getInstance().start();

    advect();

    Glb::Timer::getInstance().recordTime("advect");

    addExternalForces();

    Glb::Timer::getInstance().recordTime("external forces");

    project();

    Glb::Timer::getInstance().recordTime("project");

}
```
