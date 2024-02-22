#include "Mac2dComponent.h"

namespace FluidSimulation {
    namespace MAC2d {
        void Mac2dComponent::shutDown() {
            delete renderer, solver, grid;
            renderer = NULL;
            solver = NULL;
            grid = NULL;
        }

        void Mac2dComponent::init() {

            if (renderer != NULL || solver != NULL || grid != NULL) {
                delete renderer, solver, grid;
                renderer = NULL;
                solver = NULL;
                grid = NULL;
            }
            grid = new MACGrid2d();
            renderer = new Renderer(*grid);
            solver = new Solver(*grid);
        }

        void Mac2dComponent::simulate() {
            solver->solve();
        }

        GLuint Mac2dComponent::getRenderedTexture()
        {
            return renderer->getImTextureIDByDensity();
        }
    }
}