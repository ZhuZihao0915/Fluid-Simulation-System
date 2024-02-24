#include "MAC/include/Solver.h"
#include <GL/glut.h>
#include "ConjGrad2d.h"
#include "Configure.h"

namespace FluidSimulation {
    namespace MAC2d {
        Solver::Solver(MACGrid2d& grid) : mGrid(grid)
        {
            mGrid.reset();
            constructA();
            constructPrecon();
        }

        Solver::~Solver()
        {

        }

        void Solver::solve()
        {
            target.reset();

            // advect 速度
            advectVelocity();
            // 考虑浮力、涡度等因素
            addExternalForces();
            // project 确保incompressible的特性
            project();

            // advect 温度
            advectTemperature();

            // advect 密度
            advectDensity();
        }

        void Solver::constructA() {
            unsigned int numCells = mGrid.mSolid.data().size();
            A.resize(numCells, numCells, false);

            // 矩阵A(r, c)存储着 cell r 接受到 cell c 带来的压力
            for (unsigned int row = 0; row < numCells; row++)
            {
                int ri, rj; mGrid.getCell(row, ri, rj); // Each row corresponds to a cell
                if (mGrid.isSolidCell(ri, rj)) continue;
                for (unsigned int col = 0; col < numCells; col++)
                {
                    int ci, cj; mGrid.getCell(col, ci, cj); // Each col corresponds to a possible neighbor
                    if (mGrid.isSolidCell(ci, cj)) continue;
                    double coeff = mGrid.getPressureCoeffBetweenCells(ri, rj, ci, cj);
                    if (fabs(coeff) > 0.0001)
                    {
                        A(row, col) = coeff;
                    }
                }
            }
        }

#define VALA(r,c) (r != -1 && c != -1)? A(r,c) : 0
        void Solver::constructPrecon() {
            precon.resize(A.size1());
            std::fill(precon.begin(), precon.end(), 0);

            double tau = 0.0; // Disable MIC(0) 0.97;
            for (unsigned int index = 0; index < A.size1(); index++)
            {
                int i, j; mGrid.getCell(index, i, j);
                if (mGrid.isSolidCell(i, j)) continue;

                int neighbori = mGrid.getIndex(i - 1, j);
                int neighborj = mGrid.getIndex(i, j - 1);
                double termi = neighbori != -1 ? A(index, neighbori) * precon(neighbori) : 0;
                double termj = neighborj != -1 ? A(index, neighborj) * precon(neighborj) : 0;

                double termii = 0;
                if (neighbori != -1)
                {
                    int neighborij = mGrid.getIndex(i - 1, j + 1);
                    double termii0 = (VALA(neighbori, neighborij));
                    double termii1 = precon(neighbori) * precon(neighbori);
                    termii = VALA(index, neighbori) * termii0 / termii1;

                }

                double termjj = 0;
                if (neighborj != -1)
                {
                    int neighborji = mGrid.getIndex(i + 1, j - 1);
                    double termjj0 = (VALA(neighborj, neighborji));
                    double termjj1 = precon(neighborj) * precon(neighborj);
                    termjj = VALA(index, neighborj) * termjj0 / termjj1;
                }

                double e = A(index, index) - termi * termi - termj * termj
                    - tau * (termii + termjj);

                precon(index) = 1 / sqrt(e);
            }
        }

        void Solver::constructB(unsigned int numCells) {
            b.resize(numCells);
            double constant = -(MAC2dPara::airDensity * MAC2dPara::theCellSize2d * MAC2dPara::theCellSize2d) / MAC2dPara::dt;
            for (unsigned int index = 0; index < numCells; index++)
            {
                int i, j; mGrid.getCell(index, i, j);
                if (!mGrid.isSolidCell(i, j))
                    b(index) = constant * mGrid.getDivergence(i, j);
                else b(index) = 0;
            }
        }

#define FOR_EACH_CELL \
    for(int j = 0; j < MAC2dPara::theDim2d[MACGrid2d::Y]; j++) \
        for(int i = 0; i < MAC2dPara::theDim2d[MACGrid2d::X]; i++) 

#define FOR_EACH_LINE \
    for(int j = 0; j < MAC2dPara::theDim2d[MACGrid2d::Y]+1; j++) \
        for(int i = 0; i < MAC2dPara::theDim2d[MACGrid2d::X]+1; i++)

        void Solver::advectVelocity() {
            FOR_EACH_LINE
            {
                // advect u
                if (mGrid.isFace(i,j,mGrid.X))   // 确保在水平方向的界内
                {
                   glm::vec2 pos = mGrid.getLeftLine(i,j);         // 左边中点，即存储x方向速度的点  
                   glm::vec2 newpos = mGrid.traceBack(pos, MAC2dPara::dt);    // 回溯位置
                   glm::vec2 newvel = mGrid.getVelocity(newpos);   // 得到回溯的位置的速度
                   target.mU(i, j) = newvel[mGrid.X];              // 赋值，但只使用水平方向的分量来赋值
                }
            // advect v
            if (mGrid.isFace(i,j, mGrid.Y))
            {
               glm::vec2 pos = mGrid.getBottomLine(i,j);
               glm::vec2 newpos = mGrid.traceBack(pos, MAC2dPara::dt);
               glm::vec2 newvel = mGrid.getVelocity(newpos);
               target.mV(i, j) = newvel[mGrid.Y];
            }
            }

            mGrid.mU = target.mU;
            mGrid.mV = target.mV;
        }


        void Solver::addExternalForces() {
            // 浮力
            FOR_EACH_LINE
            {
               if (mGrid.isFace(i,j,mGrid.Y))
               {
                  glm::vec2 pos = mGrid.getBottomLine(i,j);
                  double yforce = mGrid.getBoussinesqForce(pos);
                  double vel = mGrid.mV(i,j);
                  vel = vel + MAC2dPara::dt * yforce;
                  target.mV(i, j) = vel;
               }
            }
            mGrid.mV = target.mV;

            // 涡度
            // 初始化力的网格
            Glb::GridData2d forcesX, forcesY;
            forcesX.initialize();
            forcesY.initialize();

            // 遍历网格，计算涡度约束力
            FOR_EACH_CELL
            {
                double force = mGrid.getConfinementForce(i,j);
                forcesX(i,j) = force;
                forcesY(i,j) = force;
            }

                // 使用计算好的约束力，更新速度场
                FOR_EACH_LINE
            {
                if (mGrid.isFace(i, j, mGrid.X))
                {
                    glm::vec2 pos = mGrid.getLeftLine(i, j);  // 得到左边中点
                    double vel = mGrid.mU(i, j);
                    double xforce = 0.5 * (forcesX(i, j) - forcesX(i - 1, j));
                    vel = vel + MAC2dPara::dt * xforce;
                    target.mU(i, j) = vel;
                }

                if (mGrid.isFace(i, j, mGrid.Y))
                {
                    glm::vec2 pos = mGrid.getBottomLine(i, j);
                    double yforce = 0.5 * (forcesY(i, j) - forcesY(i, j - 1));
                    double vel = mGrid.mV(i, j);
                    vel = vel + MAC2dPara::dt * yforce;
                    target.mV(i, j) = vel;
                }
            }
            mGrid.mU = target.mU;
            mGrid.mV = target.mV;
        }


        void Solver::project() {
            // Solve Ax = b for pressure
            unsigned int numCells = MAC2dPara::theDim2d[0] * MAC2dPara::theDim2d[1];
            constructB(numCells);

            ublas::vector<double> p(numCells);

            // 共轭梯度法求解泊松方程 Ap=b
            // A是压力项的系数矩阵，是一个稀疏且对称的矩阵；b为负的散度
            // 找到一个压力场p，使得应用离散拉普拉斯算子A 后，得到的结果（Ap）与速度场的散度（b）足够接近
            // 相关博客：https://yangwc.com/2019/08/03/MakingFluidImcompressible/
            // 为了进一步加快求解的收敛速度，可以采用预处理的共轭梯度法
            Glb::cg_psolve2d(A, precon, b, p, 500, 0.005);
            //Glb::cg_solve2d(A, b, p, 500, 0.005);


            // Subtract pressure from our velocity and save in target
            // u_new = u - dt*(1/theAirPressure)*((p_i+1-p_i)/theCellSize)
            double scaleConstant = MAC2dPara::dt / MAC2dPara::airDensity;
            double pressureChange;

            // 使用压力场更新速度场
            FOR_EACH_LINE
            {
               if (mGrid.isFace(i,j,mGrid.X))
               {
                  if (mGrid.isSolidFace(i, j, mGrid.X))
                  {
                     target.mU(i, j) = 0.0;
                  }
                  else
                  {
                     int index1 = mGrid.getIndex(i, j);
                     int index2 = mGrid.getIndex(i - 1, j);
                     pressureChange = (p(index1) - p(index2)) / MAC2dPara::theCellSize2d;
                     double vel = mGrid.mU(i,j);
                     vel = vel - scaleConstant * pressureChange;
                     target.mU(i, j) = vel;
                  }

               }
               if (mGrid.isFace(i,j, mGrid.Y))
               {
                   // Hard-code boundary condition for now
                   if (mGrid.isSolidFace(i,j, mGrid.Y))
                   {
                      target.mV(i, j) = 0.0;
                   }
                   else
                   {
                      int index1 = mGrid.getIndex(i,j);
                      int index2 = mGrid.getIndex(i,j - 1);
                      pressureChange = (p(index1) - p(index2)) / MAC2dPara::theCellSize2d;
                      double vel = mGrid.mV(i,j);
                      vel = vel - scaleConstant * pressureChange;
                      target.mV(i, j) = vel;
                   }
                }
            }

            mGrid.mU = target.mU;
            mGrid.mV = target.mV;
            assert(mGrid.checkDivergence());
        }

        void Solver::advectTemperature() {
            FOR_EACH_CELL
            {
               glm::vec2 pos = mGrid.getCenter(i,j);
               glm::vec2 newpos = mGrid.traceBack(pos, MAC2dPara::dt);
               double newt = mGrid.getTemperature(newpos);
               target.mT(i,j) = newt;
            }
            mGrid.mT = target.mT;
        }

        void Solver::advectDensity() {
            FOR_EACH_CELL
            {
               glm::vec2 pos = mGrid.getCenter(i,j);
               glm::vec2 newpos = mGrid.traceBack(pos, MAC2dPara::dt);
               double newd = mGrid.getDensity(newpos);
               target.mD(i,j) = newd;
            }
            mGrid.mD = target.mD;
        }

    }
}
