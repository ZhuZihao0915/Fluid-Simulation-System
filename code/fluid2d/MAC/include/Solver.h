#ifndef __MAC2D_SOLVER_H__
#define __MAC2D_SOLVER_H__

#include "MAC/include/MACGrid2d.h"
#include "Configure.h"

namespace FluidSimulation{
	namespace MAC2d {
		class Solver {
		public:
			Solver(MACGrid2d* grid);
			virtual ~Solver();

			virtual void reset();
			virtual void solve();
			virtual void setGridDimensions(int x, int y);

			// 
			void updateSources();
			void advectVelocity();
			void addExternalForces();
			void project();
			void advectTemperature();
			void advectDensity();

		protected:
			MACGrid2d* mGrid;
		};
	}
}

#endif // !__EULER_SOLVER_H__
