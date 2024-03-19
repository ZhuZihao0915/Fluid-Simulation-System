#include "Eulerian3dComponent.h"

namespace FluidSimulation {
    namespace Eulerian3d {
        void Eulerian3dComponent::shutDown() {
            delete renderer, solver, grid;
            renderer = NULL;
            solver = NULL;
            grid = NULL;
        }

        void Eulerian3dComponent::init() {

            if (renderer != NULL || solver != NULL || grid != NULL) {
                shutDown();
            }

            Glb::Timer::getInstance().clear();

            grid = new MACGrid3d();
            renderer = new Renderer(*grid);
            solver = new Solver(*grid);
        }

        void Eulerian3dComponent::simulate() {
            grid->updateSources();
            solver->solve();
        }

        GLuint Eulerian3dComponent::getRenderedTexture()
        {
            if (simulating) { Glb::Timer::getInstance().start(); }
            renderer->draw();
            if (simulating) { Glb::Timer::getInstance().recordTime("rendering"); }
            
            return renderer->getTextureID();
        }

    }
}