#ifndef __MAC3D_SOLVER_H__
#define __MAC3D_SOLVER_H__

#pragma warning(disable: 4244 4267 4996)
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

#include "MACGrid3d.h"
#include "Configure.h"

namespace FluidSimulation{
	namespace MAC3d {
		class Solver {
		public:
			Solver(MACGrid3d& grid);
			virtual ~Solver();

			virtual void solve();

			// 
			void updateSources();
			void advectVelocity();
			void addExternalForces();
			void project();
			void advectTemperature();
			void advectDensity();

			void constructA();
			void constructB(unsigned int numCells);
			void constructPrecon();

			ublas::vector<double> conjugateGradient(const ublas::compressed_matrix<double>& A, const ublas::vector<double>& b, const ublas::vector<double>& x0, const double tol = 1e-10, const int max_iter = 1000) {
				int n = b.size();
				ublas::vector<double> x = x0;
				ublas::vector<double> r = b - ublas::prod(A, x);
				ublas::vector<double> p = r;
				ublas::vector<double> Ap(n);

				double alpha, beta, rsold, rsnew;
				rsold = ublas::inner_prod(r, r);

				for (int i = 0; i < max_iter; ++i) {
					Ap = ublas::prod(A, p);
					alpha = rsold / ublas::inner_prod(p, Ap);
					x += alpha * p;
					r -= alpha * Ap;
					rsnew = ublas::inner_prod(r, r);
					if (sqrt(rsnew) < tol)
						break;
					beta = rsnew / rsold;
					p = r + beta * p;
					rsold = rsnew;
				}

				return x;
			}

		protected:
			MACGrid3d& mGrid;

			MACGrid3d target;   // 用于advection阶段存储新的场

			ublas::compressed_matrix<double> A;
			ublas::vector<double> b;
			ublas::vector<double> precon;
		};
	}
}

#endif // !__EULER_SOLVER_H__
