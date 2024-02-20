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
            renderer->init();
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

        void Mac3dComponent::cameraMove(float x, float y) {
            renderer->mCamera.ProcessMove(glm::vec2(x, y));
        }

        void Mac3dComponent::cameraRotate(float x, float y) {
            renderer->mCamera.ProcessRotate(glm::vec2(x, y));
        }

        void Mac3dComponent::cameraScale(float w) {
            renderer->mCamera.ProcessScale(static_cast<float>(w));
        }

    }
}