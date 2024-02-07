#include "MACGrid2d.h"
#include "GL/glut.h"
#include "ConjGrad2d.h"
#include "Configure.h"
#include <math.h>
#include <map>
#include <stdio.h>


namespace FluidSimulation {
    namespace MAC2d {

        ublas::compressed_matrix<double> A;
        ublas::vector<double> precon;

        // Global
        MACGrid2d target;   // 用于advection阶段存储新的场

        // NOTE: x -> cols, z -> rows, y -> stacks
        MACGrid2d::RenderMode MACGrid2d::theRenderMode = SHEETS;
        bool MACGrid2d::theDisplayVel = true;
        double MACGrid2d::theAirDensity = 1.0; //(1.3 kg/m^2) in reality,
        double MACGrid2d::theAmbientTemp = 0.0;
        double MACGrid2d::theBoussinesqAlpha = 500.0;
        double MACGrid2d::theBoussinesqBeta = 2500.0;
        double MACGrid2d::theVorticityConst = 100.0;
        bool MACGrid2d::theVConfEnabled = true;
        bool MACGrid2d::theSourceEnabled = true;

#define FOR_EACH_CELL \
    for(int j = 0; j < theDim2d[MACGrid2d::Y]; j++) \
        for(int i = 0; i < theDim2d[MACGrid2d::X]; i++) 

#define FOR_EACH_LINE \
    for(int j = 0; j < theDim2d[MACGrid2d::Y]+1; j++) \
        for(int i = 0; i < theDim2d[MACGrid2d::X]+1; i++)
        // 分别加一，适配 y 方向和 x 方向的速度


#ifndef __MINMAX_DEFINED
#  define max(a,b)    (((a) > (b)) ? (a) : (b))
#  define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif


        glm::vec2& clamp(glm::vec2& pos)
        {
            pos[0] = max(0.0, min(theDim2d[0] * theCellSize2d, pos[0]));
            pos[1] = max(0.0, min(theDim2d[1] * theCellSize2d, pos[1]));
            return pos;
        }

        MACGrid2d::MACGrid2d()
        {
            initialize();
        }

        MACGrid2d::MACGrid2d(const MACGrid2d& orig)
        {
            mU = orig.mU;
            mV = orig.mV;
            mD = orig.mD;
            mT = orig.mT;
            mSolid = orig.mSolid;
        }

        MACGrid2d& MACGrid2d::operator=(const MACGrid2d& orig)
        {
            if (&orig == this)
            {
                return *this;
            }
            mU = orig.mU;
            mV = orig.mV;
            mD = orig.mD;
            mT = orig.mT;
            mSolid = orig.mSolid;

            return *this;
        }

        MACGrid2d::~MACGrid2d()
        {
        }

        void MACGrid2d::reset()
        {
            mU.initialize(0.0);
            mV.initialize(0.0);
            mD.initialize(0.0);
            mT.initialize(theAmbientTemp);

            // Set default vel to make things more interesting and avoid degenerate fluid case
         //#ifndef _DEBUG
            int sourcei = (int)theDim2d[0] / 2;
            mU(sourcei, 0) = cos(0.3);
            mV(sourcei, 0) = sin(0.3);
            //#endif
        }

        void MACGrid2d::createSolids()
        {
            mSolid.initialize();

            /*int j = theDim2d[1] / 2;
            for (int i = theDim2d[0] / 4; i < theDim2d[0] * 3 / 4; i++) {
                mSolid(i, j) = 1;
            }*/
        }

        void MACGrid2d::initialize()
        {
            reset();
            createSolids();
            constructA(A);
            constructPrecon(A, precon);

            //testInterpolation();
            //assert(checkDivergence());
        }

        void MACGrid2d::updateSources()
        {
            // 初始化更新时用到的临时场
            target.reset();
            
            // source的位置
            int sourcei = (int)theDim2d[0] / 5;

            // 添加温度、密度
            mT(sourcei, 0) = 2.0;
            mD(sourcei, 0) = 2.0;

            // 赋予初始垂直方向速度
            mV(sourcei, 0) = 1.0;
        }

        void MACGrid2d::advectVelocity(double dt)
        {
            FOR_EACH_LINE
            {
               // advect u
               if (isFace(i,j,X))   // 确保在水平方向的界内
               {
                  glm::vec2 pos = getLeftLine(i,j);         // 左边中点，即存储x方向速度的点  
                  glm::vec2 newpos = traceBack(pos, dt);    // 回溯位置
                  glm::vec2 newvel = getVelocity(newpos);   // 得到回溯的位置的速度
                  target.mU(i, j) = newvel[X];              // 赋值，但只使用水平方向的分量来赋值
               }
               // advect v
               if (isFace(i,j,Y))
               {
                  glm::vec2 pos = getBottomLine(i,j);
                  glm::vec2 newpos = traceBack(pos, dt);
                  glm::vec2 newvel = getVelocity(newpos);
                  target.mV(i, j) = newvel[Y];
               }
            }

            mU = target.mU;
            mV = target.mV;
        }

        void MACGrid2d::advectTemperature(double dt)
        {
            FOR_EACH_CELL
            {
               glm::vec2 pos = getCenter(i,j);
               glm::vec2 newpos = traceBack(pos, dt);
               double newt = getTemperature(newpos);
               target.mT(i,j) = newt;
            }
            mT = target.mT;
        }

        void MACGrid2d::advectDensity(double dt)
        {
            FOR_EACH_CELL
            {
               glm::vec2 pos = getCenter(i,j);
               glm::vec2 newpos = traceBack(pos, dt);
               double newd = getDensity(newpos);
               target.mD(i,j) = newd;
            }
            mD = target.mD;
        }

        // 根据温度差异计算 Boussinesq Force
        double MACGrid2d::getBoussinesqForce(const glm::vec2& pos)
        {
            // Use Boussinesq approximation
            // f = [0, -alpha*smokeDensity + beta*(T - T_amb), 0]
            double temperature = getTemperature(pos);
            double smokeDensity = getDensity(pos);

            double yforce = -theBoussinesqAlpha * smokeDensity +
                theBoussinesqBeta * (temperature - theAmbientTemp);

            return yforce;
        }

        // 计算浮力，更新向上的速度
        void MACGrid2d::computeBouyancy(double dt)
        {
            FOR_EACH_LINE
            {
               if (isFace(i,j,Y))
               {
                  glm::vec2 pos = getBottomLine(i,j);
                  double yforce = getBoussinesqForce(pos);
                  double vel = mV(i,j);
                  vel = vel + dt * yforce;
                  target.mV(i, j) = vel;
               }
            }
            mV = target.mV;
        }

        glm::vec2 MACGrid2d::getVorticityN(int i, int j)
        {
            glm::vec2 right = getVorticity(i + 1, j);
            glm::vec2 left = getVorticity(i - 1, j);
            glm::vec2 top = getVorticity(i, j + 1);
            glm::vec2 bottom = getVorticity(i, j - 1);

            double scale = 1.0 / (2 * theCellSize2d);
            double x = scale * (glm::length(right) - glm::length(left));
            double y = scale * (glm::length(top) - glm::length(bottom));
            
            if (x == y && y == 0) {
                return glm::vec2(0, 0);
            }
            glm::vec2 N(x, y);
            return glm::normalize(N);
        }

        glm::vec2 MACGrid2d::getVorticity(int i, int j)
        {
            glm::vec2 right = getVelocity(getRightLine(i + 1, j));
            glm::vec2 left = getVelocity(getLeftLine(i, j));
            glm::vec2 top = getVelocity(getTopLine(i, j + 1));
            glm::vec2 bottom = getVelocity(getBottomLine(i, j));

            double scale = 1.0 / (theCellSize2d);
            double x = scale * (top.y - bottom.y);
            double y = scale * (right.x - left.x);

            return glm::vec2(x, y);
        }

        // 计算涡度约束力
        double MACGrid2d::getConfinementForce(int i, int j)
        {
            glm::vec2 N = getVorticityN(i, j);  // 涡度法线
            glm::vec2 w = getVorticity(i, j);   // 涡度
            double crossProduct = N.x * w.y - N.y * w.x;
            return theVorticityConst * theCellSize2d * crossProduct;
        }

        void MACGrid2d::computeVorticityConfinement(double dt)
        {
            // 初始化力的网格
            Glb::GridData2d forcesX, forcesY;
            forcesX.initialize();
            forcesY.initialize();

            // 遍历网格，计算涡度约束力
            FOR_EACH_CELL
            {
                double force = getConfinementForce(i,j);
                forcesX(i,j) = force;
                forcesY(i,j) = force;
            }

            // 使用计算好的约束力，更新速度场
            FOR_EACH_LINE
            {
                if (isFace(i, j, X))
                {
                    glm::vec2 pos = getLeftLine(i, j);  // 得到左边中点
                    double vel = mU(i, j);
                    double xforce = 0.5 * (forcesX(i, j) - forcesX(i - 1, j));
                    vel = vel + dt * xforce;
                    target.mU(i, j) = vel;
                }

                if (isFace(i, j, Y))
                {
                    glm::vec2 pos = getBottomLine(i, j);
                    double yforce = 0.5 * (forcesY(i, j) - forcesY(i, j - 1));
                    double vel = mV(i, j);
                    vel = vel + dt * yforce;
                    target.mV(i, j) = vel;
                }
            }
            mU = target.mU;
            mV = target.mV;
        }


        void MACGrid2d::addExternalForces(double dt)
        {
            // 浮力
            computeBouyancy(dt);
            
            // 涡度
            if (theVConfEnabled) computeVorticityConfinement(dt);
        }

        double MACGrid2d::checkDivergence(int i, int j)
        {
            double x1 = mU(i + 1, j);
            double x0 = mU(i, j);
            double y1 = mV(i, j + 1);
            double y0 = mV(i, j);
            double xdiv = x1 - x0;
            double ydiv = y1 - y0;
            double div = (xdiv + ydiv) / theCellSize2d;
            return div;
        }

        // 计算散度
        double MACGrid2d::getDivergence(int i, int j)
        {

            double x1 = isSolidCell(i + 1, j) ? 0.0 : mU(i + 1, j);
            double x0 = isSolidCell(i - 1, j) ? 0.0 : mU(i, j);

            double y1 = isSolidCell(i, j + 1) ? 0.0 : mV(i, j + 1);
            double y0 = isSolidCell(i, j - 1) ? 0.0 : mV(i, j);

            double xdiv = x1 - x0;
            double ydiv = y1 - y0;
            double div = (xdiv + ydiv) / theCellSize2d;

            return div;
        }

        // b为负的散度
        void MACGrid2d::constructB(ublas::vector<double>& b, unsigned int numCells, double dt)
        {
            double constant = -(theAirDensity * theCellSize2d * theCellSize2d) / dt;
            for (unsigned int index = 0; index < numCells; index++)
            {
                int i, j; getCell(index, i, j);
                if (!isSolidCell(i, j))
                    b(index) = constant * getDivergence(i, j);
                else b(index) = 0;
            }
        }

#define VALA(r,c) (r != -1 && c != -1)? A(r,c) : 0
        void MACGrid2d::constructPrecon(ublas::compressed_matrix<double>& A,
            ublas::vector<double>& precon)
        {
            precon.resize(A.size1());
            std::fill(precon.begin(), precon.end(), 0);

            double tau = 0.0; // Disable MIC(0) 0.97;
            for (unsigned int index = 0; index < A.size1(); index++)
            {
                int i, j; getCell(index, i, j);
                if (isSolidCell(i, j)) continue;

                int neighbori = getIndex(i - 1, j);
                int neighborj = getIndex(i, j - 1);
                double termi = neighbori != -1 ? A(index, neighbori) * precon(neighbori) : 0;
                double termj = neighborj != -1 ? A(index, neighborj) * precon(neighborj) : 0;

                double termii = 0;
                if (neighbori != -1)
                {
                    int neighborij = getIndex(i - 1, j + 1);
                    double termii0 = (VALA(neighbori, neighborij));
                    double termii1 = precon(neighbori) * precon(neighbori);
                    termii = VALA(index, neighbori) * termii0 / termii1;

                }

                double termjj = 0;
                if (neighborj != -1)
                {
                    int neighborji = getIndex(i + 1, j - 1);
                    double termjj0 = (VALA(neighborj, neighborji));
                    double termjj1 = precon(neighborj) * precon(neighborj);
                    termjj = VALA(index, neighborj) * termjj0 / termjj1;
                }

                double e = A(index, index) - termi * termi - termj * termj
                    - tau * (termii + termjj);

                precon(index) = 1 / sqrt(e);
            }

#ifdef _DEBUG
            /*
           printf("E---------------------------\n");
           for (unsigned int i = 0; i < precon.size(); i++)
           {
              for (unsigned int j = 0; j < precon.size(); j++)
              {
                 if (i == j) std::cout << precon(i) << " " ;
                 else std::cout << "0 " ;
              }
              std::cout  << std::endl;
           }*/
#endif
        }

        void MACGrid2d::constructA(ublas::compressed_matrix<double>& A)
        {
            unsigned int numCells = mSolid.data().size();
            A.resize(numCells, numCells, false);

            // 矩阵A(r, c)存储着 cell r 接受到 cell c 带来的压力
            for (unsigned int row = 0; row < numCells; row++)
            {
                int ri, rj; getCell(row, ri, rj); // Each row corresponds to a cell
                if (isSolidCell(ri, rj)) continue;
                for (unsigned int col = 0; col < numCells; col++)
                {
                    int ci, cj; getCell(col, ci, cj); // Each col corresponds to a possible neighbor
                    if (isSolidCell(ci, cj)) continue;
                    double coeff = getPressureCoeffBetweenCells(ri, rj, ci, cj);
                    if (fabs(coeff) > 0.0001)
                    {
                        A(row, col) = coeff;
                    }
                }
            }
        }

        void MACGrid2d::project(double dt)
        {
            // Solve Ax = b for pressure
            unsigned int numCells = theDim2d[0] * theDim2d[1];
            ublas::vector<double> b(numCells);
            constructB(b, numCells, dt);

            ublas::vector<double> p(numCells);
            
            // 共轭梯度法求解泊松方程 Ap=b
            // A是压力项的系数矩阵，是一个稀疏且对称的矩阵；b为负的散度
            // 找到一个压力场p，使得应用离散拉普拉斯算子A 后，得到的结果（Ap）与速度场的散度（b）足够接近
            // 相关博客：https://yangwc.com/2019/08/03/MakingFluidImcompressible/
            // 为了进一步加快求解的收敛速度，可以采用预处理的共轭梯度法
            Glb::cg_psolve2d(A, precon, b, p, 500, 0.005);
            // Glb::cg_solve2d(A, b, p, 500, 0.005);
            

            // Subtract pressure from our velocity and save in target
            // u_new = u - dt*(1/theAirPressure)*((p_i+1-p_i)/theCellSize)
            double scaleConstant = dt / theAirDensity;
            double pressureChange;

            // 使用压力场更新速度场
            FOR_EACH_LINE
            {
               if (isFace(i,j,X))
               {
                  if (isSolidFace(i, j, X))
                  {
                     target.mU(i, j) = 0.0;
                  }
                  else
                  {
                     int index1 = getIndex(i, j);
                     int index2 = getIndex(i - 1, j);
                     pressureChange = (p(index1) - p(index2)) / theCellSize2d;
                     double vel = mU(i,j);
                     vel = vel - scaleConstant * pressureChange;
                     target.mU(i, j) = vel;
                  }

               }
               if (isFace(i,j,Y))
               {
                   // Hard-code boundary condition for now
                   if (isSolidFace(i,j,Y))
                   {
                      target.mV(i, j) = 0.0;
                   }
                   else
                   {
                      int index1 = getIndex(i,j);
                      int index2 = getIndex(i,j - 1);
                      pressureChange = (p(index1) - p(index2)) / theCellSize2d;
                      double vel = mV(i,j);
                      vel = vel - scaleConstant * pressureChange;
                      target.mV(i, j) = vel;
                   }
                }
            }

            mU = target.mU;
            mV = target.mV;
            assert(checkDivergence());
        }

        bool MACGrid2d::checkDivergence()
        {
            FOR_EACH_CELL
            {
               double div = checkDivergence(i, j);
               if (fabs(div) > 0.01)
               {
                  printf("Divergence(%d,%d,%d) = %.2f\n", i, j, div);
                  return false;
               }
            }
            return true;
        }

        glm::vec2 MACGrid2d::traceBack(const glm::vec2& pt, double dt)
        {
            glm::vec2 vel = getVelocity(pt);
            glm::vec2 pos = pt - vel * (float)dt;   // 回溯dt时间之前的位置

            // 确保在容器内
            pos[0] = max(0.0, min((theDim2d[0] - 1) * theCellSize2d, pos[0]));
            pos[1] = max(0.0, min((theDim2d[1] - 1) * theCellSize2d, pos[1]));

            // 确保不在固体内
            int i, j;
            if (inSolid(pt, i, j))
            {
                double t = 0;
                if (intersects(pt, vel, i, j, t))
                {
                    pos = pt - vel * (float)t;
                }
                else
                {
                    std::cout << "WARNING: Shouldn't get here." << std::endl;
                }
            }
            return pos;
        }

        bool MACGrid2d::intersects(const glm::vec2& wPos, const glm::vec2& wDir, int i, int j, double& time)
        {
            // Transform pos/dir to local coordinates
            // Cell needs to be translated to origin and scaled by 1/theCellSize
            glm::vec2 pos = getCenter(i, j);

            glm::vec2 rayStart = wPos - pos;  // * => transform vector
            glm::vec2 rayDir = wDir;  // ^ => transform vector; the symbol was chosen arbitrarily

            // Calculate ray/box intersection test using slabs method
            double tmin = -9999999999.0;
            double tmax = 9999999999.0;

            // Min/Max is the same in every direction for our cube
            double min = -0.5 * theCellSize2d;
            double max = 0.5 * theCellSize2d;

            // For X,Y,Z planes, find intersection with the minimum/maximum boundary values
            // The clever part: the maximum closest intersection will be our cube intersection
            for (int i = 0; i < 3; i++)
            {
                double e = rayStart[i];
                double f = rayDir[i];
                if (fabs(f) > 0.000000001) // Not in ith plane
                {
                    double t1 = (min - e) / f;
                    double t2 = (max - e) / f;
                    if (t1 > t2) std::swap(t1, t2);  // Always make t1 the closest for simplicity
                    if (t1 > tmin) tmin = t1;
                    if (t2 < tmax) tmax = t2;
                    if (tmin > tmax) return false;
                    if (tmax < 0) return false;
                }
                // the ray is parallel: check if we're inside the slabs or outside
                else if (e < min || e > max) return false;
            }

            if (tmin >= 0)
            {
                time = tmin;
                return true;
            }
            else
            {
                time = tmax;
                return true;
            }
            return false;
        }

        int MACGrid2d::getIndex(int i, int j)
        {
            if (i < 0 || i > theDim2d[0] - 1) return -1;
            if (j < 0 || j > theDim2d[1] - 1) return -1;

            int col = i;
            int row = j * theDim2d[0];
            return col + row;
        }


        void MACGrid2d::getCell(int index, int& i, int& j)
        {
            j = (int)index / theDim2d[0];      // row
            i = index - j * theDim2d[0];        // col
        }

        glm::vec2 MACGrid2d::getCenter(int i, int j)
        {
            double xstart = theCellSize2d / 2.0;
            double ystart = theCellSize2d / 2.0;

            double x = xstart + i * theCellSize2d;
            double y = ystart + j * theCellSize2d;
            return glm::vec2(x, y);
        }

        glm::vec2 MACGrid2d::getLeftLine(int i, int j)
        {
            return getCenter(i, j) - glm::vec2(theCellSize2d * 0.5, 0.0);
        }

        glm::vec2 MACGrid2d::getRightLine(int i, int j)
        {
            return getCenter(i, j) + glm::vec2(theCellSize2d * 0.5, 0.0);
        }

        glm::vec2 MACGrid2d::getTopLine(int i, int j)
        {
            return getCenter(i, j) + glm::vec2(0.0, theCellSize2d * 0.5);
        }

        glm::vec2 MACGrid2d::getBottomLine(int i, int j)
        {
            return getCenter(i, j) - glm::vec2(0.0, theCellSize2d * 0.5);
        }


        glm::vec2 MACGrid2d::getVelocity(const glm::vec2& pt)
        {
            if (inSolid(pt))
            {
                //pt.Print("WARNING: Velocity given point in solid");
                return glm::vec2(0,0);
            }

            glm::vec2 vel;
            vel[0] = getVelocityX(pt);
            vel[1] = getVelocityY(pt);
            return vel;
        }

        double MACGrid2d::getVelocityX(const glm::vec2& pt)
        {
            return mU.interpolate(pt);
        }

        double MACGrid2d::getVelocityY(const glm::vec2& pt)
        {
            return mV.interpolate(pt);
        }

        double MACGrid2d::getTemperature(const glm::vec2& pt)
        {
            return mT.interpolate(pt);
        }

        double MACGrid2d::getDensity(const glm::vec2& pt)
        {
            return mD.interpolate(pt);
        }

        int MACGrid2d::numSolidCells()
        {
            int numSolid = 0;
            FOR_EACH_CELL{ numSolid += mSolid(i,j); }
            return numSolid;
        }

        bool MACGrid2d::inSolid(const glm::vec2& pt)
        {
            int i, j, k;
            mSolid.getCell(pt, i, j);
            return isSolidCell(i, j) == 1;
        }

        bool MACGrid2d::inSolid(const glm::vec2& pt, int& i, int& j)
        {
            mSolid.getCell(pt, i, j);
            return isSolidCell(i, j) == 1;
        }

        int MACGrid2d::isSolidCell(int i, int j)
        {
            bool containerBoundary = (i < 0 || i > theDim2d[0] - 1) ||
                (j < 0 || j > theDim2d[1] - 1);


            // Check interior boundaries too
            bool objectBoundary = (mSolid(i, j) == 1);

            return containerBoundary || objectBoundary ? 1 : 0;
        }

        int MACGrid2d::isSolidFace(int i, int j, MACGrid2d::Direction d)
        {
            if (d == X && (i == 0 || i == theDim2d[0])) return 1;
            else if (d == Y && (j == 0 || j == theDim2d[1])) return 1;

            if (d == X && (mSolid(i, j) || mSolid(i - 1, j))) return 1;
            if (d == Y && (mSolid(i, j) || mSolid(i, j - 1))) return 1;

            return 0;
        }

        bool MACGrid2d::isNeighbor(int i0, int j0, int i1, int j1)
        {
            if (abs(i0 - i1) == 1 && j0 == j1 ) return true;
            if (abs(j0 - j1) == 1 && i0 == i1 ) return true;
            return false;
        }

        double MACGrid2d::getPressureCoeffBetweenCells(
            int i, int j, int pi, int pj)
        {
            // 同一个cell
            if (i == pi && j == pj ) // self
            {
                int numSolidNeighbors = (isSolidCell(i + 1, j) +
                    isSolidCell(i - 1, j) +
                    isSolidCell(i, j + 1) +
                    isSolidCell(i, j - 1));
                // Return number of non-solid boundaries around cel ij
                return 4.0 - numSolidNeighbors; // 二维，4
            }
            // 如果两个是邻居且后者为固体，error
            if (isNeighbor(i, j, pi, pj) && !isSolidCell(pi, pj)) return -1.0;
            // 不是邻居
            return 0.0;
        }


        glm::vec4 MACGrid2d::getRenderColor(int i, int j)
        {
            double value = mD(i, j);
            return glm::vec4(1.0, 1.0, 1.0, value);
        }

        glm::vec4 MACGrid2d::getRenderColor(const glm::vec2& pt)
        {
            double value = getDensity(pt);
            return glm::vec4(value, value, value, value);
        }

        // 确保在界内
        bool MACGrid2d::isFace(int i, int j, MACGrid2d::Direction d)
        {
            switch (d)
            {
            case X: return (i >= 0 && i < theDim2d[X] + 1 &&
                j >= 0 && j < theDim2d[Y]);
            case Y: return (i >= 0 && i < theDim2d[X] &&
                j >= 0 && j < theDim2d[Y] + 1);
            }
            printf("Error: bad direction passed to isFace\n");
            return false;
        }

        void MACGrid2d::testInterpolation()
        {
            printf("WARNING: testing interpolations\n");
            FOR_EACH_LINE
            {
               if (isFace(i,j,X)) mU(i, j) = i * theCellSize2d;
               if (isFace(i,j,Y)) mV(i, j) = j * theCellSize2d;
            }

                FOR_EACH_CELL
            {
                mD(i, j) = i + j ;
            }
            //std::cout << "D: " << mD.data() << std::endl;

            glm::vec2 test = getVelocity(glm::vec2(0.25, 0.25));
            glm::vec2 test1 = getVelocity(glm::vec2(1.5, 2.2));
            glm::vec2 test2 = getVelocity(glm::vec2(0.0, 0.1));

            double test3 = getDensity(glm::vec2(0.0, 0.0)); // should be 0
            double test4 = getDensity(glm::vec2(0.5, 0.5));  // should be 0
            double test5 = getDensity(glm::vec2(1.0, 1.0));  // should be 1
        }

    }
}