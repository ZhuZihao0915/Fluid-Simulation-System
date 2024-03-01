#include "fluid3d/MAC/include/Solver.h"
#include <GL/glut.h>
#include "ConjGrad3d.h"
#include "Configure.h"
#include "Global.h"

namespace FluidSimulation {
    namespace MAC3d {
        Solver::Solver(MACGrid3d& grid) : mGrid(grid)
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
            // TODO
            // ���/ģ��

            target.reset();
            
            // advect �ٶ�
            advectVelocity();

            // ���Ǹ������жȵ�����
            addExternalForces();

            // project ȷ��incompressible������
            project();

            // advect �¶�
            advectTemperature();

            // advect �ܶ�
            advectDensity();

        }

#define FOR_EACH_CELL \
    for(int k =0; k < MAC3dPara::theDim3d[MACGrid3d::Z]; k++) \
        for(int j = 0; j < MAC3dPara::theDim3d[MACGrid3d::Y]; j++) \
            for(int i = 0; i < MAC3dPara::theDim3d[MACGrid3d::X]; i++) 


#define FOR_EACH_FACE \
    for(int k =0; k < MAC3dPara::theDim3d[MACGrid3d::Z] + 1; k++) \
        for(int j = 0; j < MAC3dPara::theDim3d[MACGrid3d::Y] + 1; j++) \
            for(int i = 0; i < MAC3dPara::theDim3d[MACGrid3d::X] + 1; i++)
        // �ֱ��һ������ y ����� x ������ٶ�

        void Solver::advectVelocity()
        {
#pragma omp parallel for
            FOR_EACH_FACE
            {
               if (mGrid.isFace(i,j,k,mGrid.X))
               {
                  glm::vec3 pos = mGrid.getLeftFace(i,j,k);
                  glm::vec3 newpos = mGrid.traceBack(pos, MAC3dPara::dt);
                  glm::vec3 newvel = mGrid.getVelocity(newpos);
                  target.mU(i, j, k) = newvel[mGrid.X];
               }
               if (mGrid.isFace(i,j,k, mGrid.Y))
               {
                  glm::vec3 pos = mGrid.getBottomFace(i,j,k);
                  glm::vec3 newpos = mGrid.traceBack(pos, MAC3dPara::dt);
                  glm::vec3 newvel = mGrid.getVelocity(newpos);
                  target.mV(i, j, k) = newvel[mGrid.Y];
               }
               if (mGrid.isFace(i,j,k, mGrid.Z))
               {
                  glm::vec3 pos = mGrid.getBackFace(i,j,k);
                  glm::vec3 newpos = mGrid.traceBack(pos, MAC3dPara::dt);
                  glm::vec3 newvel = mGrid.getVelocity(newpos);
                  target.mW(i, j, k) = newvel[mGrid.Z];
               }
            }

            mGrid.mU = target.mU;
            mGrid.mV = target.mV;
            mGrid.mW = target.mW;
        }

        void Solver::addExternalForces()
        {
#pragma omp parallel for
            // ���㸡�����������ϵ��ٶ�
            FOR_EACH_FACE
            {
               if (mGrid.isFace(i,j,k,mGrid.Z))
               {
                  glm::vec3 pos = mGrid.getBottomFace(i,j,k);
                  double zforce = mGrid.getBoussinesqForce(pos);
                  double vel = mGrid.mW(i,j,k);
                  vel = vel + MAC3dPara::dt * zforce;
                  target.mW(i, j, k) = vel;
               }
            }
            mGrid.mW = target.mW;


            // �����жȣ����¸�������ٶ�
            
            // ��ʼ����������
            Glb::GridData3d forcesX, forcesY, forcesZ;
            forcesX.initialize();
            forcesY.initialize();
            forcesZ.initialize();

            // �������񣬼����ж�Լ����
            FOR_EACH_CELL
            {
                glm::vec3 force = mGrid.getConfinementForce(i,j,k);
                forcesX(i, j, k) = force[0];
                forcesY(i, j, k) = force[1];
                forcesZ(i, j, k) = force[2];
            }

                // ʹ�ü���õ�Լ�����������ٶȳ�
#pragma omp parallel for
            FOR_EACH_FACE
            {
                if (mGrid.isFace(i, j, k, mGrid.X))
                {
                    glm::vec3 pos = mGrid.getLeftFace(i, j, k);
                    double vel = mGrid.mU(i, j, k);
                    double xforce = 0.5 * (forcesX(i, j, k) - forcesX(i - 1, j, k));
                    vel = vel + MAC3dPara::dt * xforce;
                    target.mU(i, j, k) = vel;
                }

                if (mGrid.isFace(i, j, k, mGrid.Y))
                {
                    glm::vec3 pos = mGrid.getBottomFace(i, j, k);
                    double yforce = 0.5 * (forcesY(i, j, k) - forcesY(i, j - 1, k));
                    double vel = mGrid.mV(i, j, k);
                    vel = vel + MAC3dPara::dt * yforce;
                    target.mV(i, j, k) = vel;
                }

                if (mGrid.isFace(i, j, k, mGrid.Z))
                {
                    glm::vec3 pos = mGrid.getBackFace(i, j, k);
                    double zforce = 0.5 * (forcesZ(i, j, k) - forcesZ(i, j, k - 1));
                    double vel = mGrid.mW(i, j, k);
                    vel = vel + MAC3dPara::dt * zforce;
                    target.mW(i, j, k) = vel;
                }
            }

            mGrid.mU = target.mU;
            mGrid.mV = target.mV;
            mGrid.mW = target.mW;
        }

        void Solver::project()
        {
            // Solve Ax = b for pressure
            unsigned int numCells = MAC3dPara::theDim3d[0] * MAC3dPara::theDim3d[1] * MAC3dPara::theDim3d[2];

            // �����ݶȷ���Ⲵ�ɷ��� Ap=b
            // A��ѹ�����ϵ��������һ��ϡ���ҶԳƵľ���bΪ����ɢ��
            // �ҵ�һ��ѹ����p��ʹ��Ӧ����ɢ������˹����A �󣬵õ��Ľ����Ap�����ٶȳ���ɢ�ȣ�b���㹻�ӽ�
            // ��ز��ͣ�https://yangwc.com/2019/08/03/MakingFluidImcompressible/
            // Ϊ�˽�һ���ӿ����������ٶȣ����Բ���Ԥ����Ĺ����ݶȷ�

            constructB(numCells);
            ublas::vector<double> p = conjugateGradient(A, b, precon);

            //Glb::cg_psolve3d(A, precon, b, p, 500, 0.005);
            // 
            //Glb::cg_solve3d(A, b, p, 500, 0.005);


            // Subtract pressure from our velocity and save in target
            // u_new = u - dt*(1/theAirPressure)*((p_i+1-p_i)/theCellSize)
            double scaleConstant = MAC3dPara::dt / MAC3dPara::airDensity;
            double pressureChange;

            // ʹ��ѹ���������ٶȳ�
#pragma omp parallel for
            FOR_EACH_FACE
            {
               if (mGrid.isFace(i,j,k,mGrid.X))
               {
                  if (mGrid.isSolidFace(i, j, k, mGrid.X))
                  {
                     target.mU(i, j, k) = 0.0;
                  }
                  else
                  {
                     int index1 = mGrid.getIndex(i,j,k);
                     int index2 = mGrid.getIndex(i - 1,j,k);
                     pressureChange = (p(index1) - p(index2)) / MAC3dPara::theCellSize3d;
                     double vel = mGrid.mU(i,j,k);
                     vel = vel - scaleConstant * pressureChange;
                     target.mU(i, j, k) = vel;
                  }

               }
               if (mGrid.isFace(i,j,k, mGrid.Y))
               {
                   // Hard-code boundary condition for now
                   if (mGrid.isSolidFace(i,j,k, mGrid.Y))
                   {
                      target.mV(i, j, k) = 0.0;
                   }
                   else
                   {
                      int index1 = mGrid.getIndex(i,j,k);
                      int index2 = mGrid.getIndex(i,j - 1,k);
                      pressureChange = (p(index1) - p(index2)) / MAC3dPara::theCellSize3d;
                      double vel = mGrid.mV(i,j,k);
                      vel = vel - scaleConstant * pressureChange;
                      target.mV(i, j, k) = vel;
                   }
                }

                if (mGrid.isFace(i,j,k, mGrid.Z))
                {
                    // Hard-code boundary condition for now
                    if (mGrid.isSolidFace(i,j,k, mGrid.Z))
                    {
                       target.mW(i, j, k) = 0.0;
                    }
                    else
                    {
                       int index1 = mGrid.getIndex(i,j,k);
                       int index2 = mGrid.getIndex(i,j,k - 1);
                       pressureChange = (p(index1) - p(index2)) / MAC3dPara::theCellSize3d;
                       double vel = mGrid.mW(i,j,k);
                       vel = vel - scaleConstant * pressureChange;
                       target.mW(i, j, k) = vel;
                    }
                 }
            }

                //std::cout << "u: " << target.mU.data() << std::endl;
                //std::cout << "v: " << target.mV.data() << std::endl;
                //std::cout << "w: " << target.mW.data() << std::endl;

            mGrid.mU = target.mU;
            mGrid.mV = target.mV;
            mGrid.mW = target.mW;
            assert(mGrid.checkDivergence());
        }

        void Solver::advectTemperature()
        {
            FOR_EACH_CELL
            {
               glm::vec3 pos = mGrid.getCenter(i,j,k);
               glm::vec3 newpos = mGrid.traceBack(pos, MAC3dPara::dt);
               double newt = mGrid.getTemperature(newpos);
               target.mT(i,j,k) = newt;
            }
            mGrid.mT = target.mT;
        }

        void Solver::advectDensity()
        {
            FOR_EACH_CELL
            {
               glm::vec3 pos = mGrid.getCenter(i,j,k);
               glm::vec3 newpos = mGrid.traceBack(pos, MAC3dPara::dt);
               double newd = mGrid.getDensity(newpos);
               target.mD(i,j,k) = newd;
            }
            mGrid.mD = target.mD;
        }
            
        
        void Solver::constructA()
        {
            unsigned int numCells = mGrid.mSolid.data().size();
            A.resize(numCells, numCells, false);

            // ����A(r, c)�洢�� cell r ���ܵ� cell c ������ѹ��
#pragma omp parallel for
            for (unsigned int row = 0; row < numCells; row++)
            {
                int ri, rj, rk; mGrid.getCell(row, ri, rj, rk); // Each row corresponds to a cell
                if (mGrid.isSolidCell(ri, rj, rk)) continue;
#pragma omp parallel for
                for (unsigned int col = 0; col < numCells; col++)
                {
                    int ci, cj, ck; mGrid.getCell(col, ci, cj, ck); // Each col corresponds to a possible neighbor
                    if (mGrid.isSolidCell(ci, cj, ck)) continue;
                    double coeff = mGrid.getPressureCoeffBetweenCells(ri, rj, rk, ci, cj, ck);
                    if (fabs(coeff) > 0.0001)
                    {
                        A(row, col) = coeff;
                    }
                }
            }
        }

        void Solver::constructB(unsigned int numCells)
        {
            b.resize(numCells);
            double constant = -(MAC3dPara::airDensity * MAC3dPara::theCellSize3d * MAC3dPara::theCellSize3d) / MAC3dPara::dt;
#pragma omp parallel for
            for (unsigned int index = 0; index < numCells; index++)
            {
                int i, j, k; 
                mGrid.getCell(index, i, j, k);
                if (!mGrid.isSolidCell(i, j, k)) {
                    b(index) = constant * mGrid.getDivergence(i, j, k);
                }
                else {
                    b(index) = 0;
                }
            }
        }


#define VALA(r,c) (r != -1 && c != -1)? A(r,c) : 0
        // ����Ԥ������Preconditioning�����������Ƶ������������������
        void Solver::constructPrecon()
        {
            precon.resize(A.size1());
            std::fill(precon.begin(), precon.end(), 0);

            double tau = 0.0; // Disable MIC(0) 0.97;
            for (unsigned int index = 0; index < A.size1(); index++)
            {
                int i, j, k;
                mGrid.getCell(index, i, j, k);
                if (mGrid.isSolidCell(i, j, k)) continue;

                int neighbori = mGrid.getIndex(i - 1, j, k);
                int neighborj = mGrid.getIndex(i, j - 1, k);
                int neighbork = mGrid.getIndex(i, j, k - 1);
                double termi = neighbori != -1 ? A(index, neighbori) * precon(neighbori) : 0;
                double termj = neighborj != -1 ? A(index, neighborj) * precon(neighborj) : 0;
                double termk = neighbork != -1 ? A(index, neighbork) * precon(neighbork) : 0;

                double termii = 0;
                if (neighbori != -1)
                {
                    int neighborij = mGrid.getIndex(i - 1, j + 1, k);
                    int neighborik = mGrid.getIndex(i - 1, j, k + 1);
                    double termii0 = (VALA(neighbori, neighborij) + VALA(neighbori, neighborik));
                    double termii1 = precon(neighbori) * precon(neighbori);
                    termii = VALA(index, neighbori) * termii0 / termii1;
                }

                double termjj = 0;
                if (neighborj != -1)
                {
                    int neighborji = mGrid.getIndex(i + 1, j - 1, k);
                    int neighborjk = mGrid.getIndex(i, j - 1, k + 1);
                    double termjj0 = (VALA(neighborj, neighborji) + VALA(neighborj, neighborjk));
                    double termjj1 = precon(neighborj) * precon(neighborj);
                    termjj = VALA(index, neighborj) * termjj0 / termjj1;
                }

                double termkk = 0;
                if (neighbork != -1)
                {
                    int neighborki = mGrid.getIndex(i + 1, j, k - 1);
                    int neighborkj = mGrid.getIndex(i, j + 1, k - 1);
                    double termkk0 = (VALA(neighbork, neighborki) + VALA(neighbork, neighborkj));
                    double termkk1 = precon(neighbork) * precon(neighbork);
                    termkk = VALA(index, neighbork) * termkk0 / termkk1;
                }

                double e = A(index, index) - termi * termi - termj * termj - termk * termk
                    - tau * (termii + termjj + termkk);

                precon(index) = 1 / sqrt(e);
            }
        }
    }
}
