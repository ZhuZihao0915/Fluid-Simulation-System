#ifndef __MAC2D_SOLVER_H__
#define __MAC2D_SOLVER_H__

#pragma warning(disable: 4244 4267 4996)
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

#include "MAC/include/MACGrid2d.h"

namespace FluidSimulation{
	namespace MAC2d {
		class Solver {
		public:
			Solver(MACGrid2d& grid);
			virtual ~Solver();

			virtual void solve();

			void updateSources();
			void advectVelocity();
			void addExternalForces();
			void project();
			void advectTemperature();
			void advectDensity();

			void constructA();
			void constructB(unsigned int numCells);
			void constructPrecon();

		protected:
			MACGrid2d& mGrid;

			MACGrid2d target;   // 用于advection阶段存储新的场


			ublas::compressed_matrix<double> A;
			ublas::vector<double> b;
			ublas::vector<double> precon;

		};
	}
}

#endif // !__EULER_SOLVER_H__
