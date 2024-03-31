#pragma once
#ifndef __EULERIAN_3D_SOLVER_H__
#define __EULERIAN_3D_SOLVER_H__

#pragma warning(disable : 4244 4267 4996)
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

#include "MACGrid3d.h"
#include "Configure.h"

namespace FluidSimulation
{
	namespace Eulerian3d
	{
		using namespace boost::numeric;
		class Solver
		{
		public:
			Solver(MACGrid3d &grid);

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
			MACGrid3d &mGrid;

			MACGrid3d target;

			unsigned int numCells = Eulerian3dPara::theDim3d[0] * Eulerian3dPara::theDim3d[1] * Eulerian3dPara::theDim3d[2];

			ublas::compressed_matrix<double> A;
			ublas::vector<double> b;
			ublas::vector<double> precon;
		};
	}
}

#endif // !__EULERIAN_3D_SOLVER_H__
