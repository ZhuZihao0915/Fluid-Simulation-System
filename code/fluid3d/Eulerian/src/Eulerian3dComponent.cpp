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
                delete renderer, solver, grid;
                renderer = NULL;
                solver = NULL;
                grid = NULL;
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

        void Eulerian3dComponent::cameraMove(float x, float y) {
            Glb::Camera::getInstance().ProcessMove(glm::vec2(x, y));
        }

        void Eulerian3dComponent::cameraRotate(float x, float y) {
            Glb::Camera::getInstance().ProcessRotate(glm::vec2(x, y));
        }

        void Eulerian3dComponent::cameraScale(float w) {
            Glb::Camera::getInstance().ProcessScale(static_cast<float>(w));
        }

    }
}