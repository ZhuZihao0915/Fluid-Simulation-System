#include "Mac3dComponent.h"

namespace FluidSimulation {
    namespace MAC3d {
        void Mac3dComponent::shutDown() {
            delete renderer, solver, grid;
            renderer = NULL;
            solver = NULL;
            grid = NULL;
        }

        void Mac3dComponent::init() {

            if (renderer != NULL || solver != NULL || grid != NULL) {
                delete renderer, solver, grid;
                renderer = NULL;
                solver = NULL;
                grid = NULL;
            }
            grid = new MACGrid3d();
            renderer = new Renderer(*grid);
            solver = new Solver(*grid);
        }

        void Mac3dComponent::simulate() {
            solver->solve();
        }

        GLuint Mac3dComponent::getRenderedTexture()
        {
            renderer->draw();
            return renderer->getImTextureIDByDensity();
        }
    }
}