#pragma once
#ifndef __CONJGRAD_3D_H__
#define __CONJGRAD_3D_H__
/*
    共轭梯度法求解泊松方程
*/

#pragma warning(disable : 4244 4267 4996)

#include "boost/numeric/ublas/vector.hpp"
#include "boost/numeric/ublas/matrix.hpp"
#include <boost/numeric/ublas/io.hpp>

#include <utility>
#include <Logger.h>
#include "Configure.h"

namespace Glb
{
    template <typename Matrix, typename Vector>
    bool cg_solve3d(const Matrix &A, const Vector &b, Vector &x, int max_iter, double tol)
    {
        std::fill(x.begin(), x.end(), 0);

        Vector r = b;
        Vector p = b;
        Vector r_old;
        Vector temp;

        // CG loop
        for (int niter = 0; niter < max_iter; niter++)
        {
            temp = prod(A, p);
            double alpha = inner_prod(r, r) / inner_prod(p, temp);
            x += (p * alpha);
            r_old = r;
            r -= (temp * alpha);
            double residn = norm_2(r);
            if (residn < tol)
            {
                return true;
            }
            double beta = inner_prod(r, r) / inner_prod(r_old, r_old);
            p = r + p * beta;
        }

        std::cout << "Error: something goes wrong during projection" << std::endl;
        return false;
    }

    int getIndex(int i, int j, int k)
    {

        if (i < 0 || i > Eulerian3dPara::theDim3d[0] - 1)
            return -1;
        if (j < 0 || j > Eulerian3dPara::theDim3d[1] - 1)
            return -1;
        if (k < 0 || k > Eulerian3dPara::theDim3d[2] - 1)
            return -1;

        int col = i;
        int row = k * Eulerian3dPara::theDim3d[0];
        int stack = j * Eulerian3dPara::theDim3d[0] * Eulerian3dPara::theDim3d[2];
        return col + row + stack;
    }

    void getCell(int index, int &i, int &j, int &k)
    {
        j = (int)index / (Eulerian3dPara::theDim3d[0] * Eulerian3dPara::theDim3d[2]);                                   // stack
        k = (int)(index - j * Eulerian3dPara::theDim3d[0] * Eulerian3dPara::theDim3d[2]) / Eulerian3dPara::theDim3d[0]; // row
        i = index - j * Eulerian3dPara::theDim3d[0] * Eulerian3dPara::theDim3d[2] - k * Eulerian3dPara::theDim3d[0];    // col
    }

    template <typename Matrix, typename Vector>
    void applyPreconditioner3d(const Matrix &A, const Vector &precon, const Vector &r, Vector &z)
    {
        int numCells = r.size();
        ublas::vector<double> q(numCells, 0.0);

        for (int index = 0; index < numCells; ++index)
        {
            int i, j, k;
            getCell(index, i, j, k);
            double termi = 0.0, termj = 0.0, termk = 0.0;
            int neighbori = getIndex(i - 1, j, k);
            int neighborj = getIndex(i, j - 1, k);
            int neighbork = getIndex(i, j, k - 1);

            if (neighbori != -1)
                termi = A(index, neighbori) * precon(neighbori) * q(neighbori);
            if (neighborj != -1)
                termj = A(index, neighborj) * precon(neighborj) * q(neighborj);
            if (neighbork != -1)
                termk = A(index, neighbork) * precon(neighbork) * q(neighbork);

            double t = r(index) - termi - termj - termk;
            q(index) = t * precon(index);
        }

        for (int index = numCells - 1; index >= 0; --index)
        {
            int i, j, k;
            getCell(index, i, j, k);
            double termi = 0.0, termj = 0.0, termk = 0.0;
            int neighbori = getIndex(i + 1, j, k);
            int neighborj = getIndex(i, j + 1, k);
            int neighbork = getIndex(i, j, k + 1);

            if (neighbori != -1)
                termi = A(index, neighbori) * precon(index) * z(neighbori);
            if (neighborj != -1)
                termj = A(index, neighborj) * precon(index) * z(neighborj);
            if (neighbork != -1)
                termk = A(index, neighbork) * precon(index) * z(neighbork);

            double t = q(index) - termi - termj - termk;
            z(index) = t * precon(index);
        }
    }

    template <typename Matrix, typename Vector>
    bool cg_psolve3d(const Matrix &A, const Vector &precon, const Vector &b, Vector &p, int max_iter, double tol)
    {
        std::fill(p.begin(), p.end(), 0.0);
        Vector r = b;
        Vector z = b;
        Vector s = b;

        applyPreconditioner3d(A, precon, r, s);

        double sigma = inner_prod(s, r);

        for (int niter = 0; niter < max_iter; ++niter)
        {
            z = prod(A, s);
            double alpha = sigma / inner_prod(s, z);
            p += alpha * s;
            r -= alpha * z;
            double resign = norm_2(r);

            if (resign < tol)
                return true;

            applyPreconditioner3d(A, precon, r, z);

            double sigma_new = inner_prod(z, r);
            double beta = sigma_new / sigma;
            s = z + beta * s;
            sigma = sigma_new;
        }
        
        Glb::Logger::getInstance().addLog("Error: something goes wrong during projection");
        return false;
    }

}

#endif