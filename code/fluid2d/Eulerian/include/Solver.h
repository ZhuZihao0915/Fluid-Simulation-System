#pragma once
#ifndef __EULERIAN_2D_SOLVER_H__
#define __EULERIAN_2D_SOLVER_H__

#pragma warning(disable: 4244 4267 4996)
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

#include "Eulerian/include/MACGrid2d.h"
#include "Global.h"

namespace FluidSimulation{
	namespace Eulerian2d {
		using namespace boost::numeric;
		class Solver {
		public:
			Solver(MACGrid2d& grid);

			void solve();

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

			MACGrid2d target;

			ublas::compressed_matrix<double> A;
			ublas::vector<double> b;
			ublas::vector<double> precon;

		};
	}
}

#endif // !__EULER_SOLVER_H__
