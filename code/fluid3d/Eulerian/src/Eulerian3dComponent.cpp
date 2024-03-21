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

            Glb::Logger::getInstance().addLog("MAC gird created. dimension: " + std::to_string(Eulerian3dPara::theDim3d[0]) + "x"
                + std::to_string(Eulerian3dPara::theDim3d[1]) + "x"
                + std::to_string(Eulerian3dPara::theDim3d[2]) + " cell size:"
                + std::to_string(Eulerian2dPara::theCellSize2d).substr(0, 3));


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