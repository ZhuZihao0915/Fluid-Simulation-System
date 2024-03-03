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

            Glb::Timer::getInstance().clear();

            grid = new MACGrid3d();
            renderer = new Renderer(*grid);
            solver = new Solver(*grid);
        }

        void Mac3dComponent::simulate() {
            grid->updateSources();
            solver->solve();
        }

        GLuint Mac3dComponent::getRenderedTexture()
        {
            renderer->draw();
            return renderer->getTextureID();
        }

        void Mac3dComponent::cameraMove(float x, float y) {
            Glb::Camera::getInstance().ProcessMove(glm::vec2(x, y));
        }

        void Mac3dComponent::cameraRotate(float x, float y) {
            Glb::Camera::getInstance().ProcessRotate(glm::vec2(x, y));
        }

        void Mac3dComponent::cameraScale(float w) {
            Glb::Camera::getInstance().ProcessScale(static_cast<float>(w));
        }

    }
}