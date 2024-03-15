#include "Eulerian2dComponent.h"

namespace FluidSimulation {
    namespace Eulerian2d {

        void Eulerian2dComponent::shutDown() {
            delete renderer, solver, grid;
            renderer = NULL;
            solver = NULL;
            grid = NULL;
        }

        void Eulerian2dComponent::init() {

            if (renderer != NULL || solver != NULL || grid != NULL) {
                delete renderer, solver, grid;
                renderer = NULL;
                solver = NULL;
                grid = NULL;
            }

            Glb::Timer::getInstance().clear();

            grid = new MACGrid2d();
            renderer = new Renderer();
            solver = new Solver(*grid);

        }

        void Eulerian2dComponent::simulate() {
            grid->updateSources();
            solver->solve();
        }

        GLuint Eulerian2dComponent::getRenderedTexture()
        {
            if (simulating) { Glb::Timer::getInstance().start(); }
            renderer->draw(*grid);
            if (simulating) { Glb::Timer::getInstance().recordTime("rendering"); }
            return renderer->getTextureID();
        }
    }
}