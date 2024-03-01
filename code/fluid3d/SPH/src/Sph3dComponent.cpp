#include "Sph3dComponent.h"
#include "Sph3dComponent.h"

namespace FluidSimulation {
    namespace SPH3d {

        void Sph3dComponent::shutDown() {
            delete renderer, solver, ps;
            renderer = NULL;
            solver = NULL;
            ps = NULL;
        }

        void Sph3dComponent::init() {
            if (renderer != NULL || solver != NULL || ps != NULL) {
                delete renderer, solver, ps;
                renderer = NULL;
                solver = NULL;
                ps = NULL;
            }
            
            renderer = new Renderer();
            renderer->Init();

            ps = new ParticalSystem3d();
            ps->setContainerSize(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1, 1, 1));
            ps->addFluidBlock(glm::vec3(0.05, 0.05, 0.4), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.0, 0.0, -3.0), 0.02);
            ps->addFluidBlock(glm::vec3(0.45, 0.45, 0.4), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.0, 0.0, -3.0), 0.02);
            
            // 对粒子排序，并组建block结构
            ps->updateBlockInfo();
            std::cout << "partical num = " << ps->mParticalInfos.size() << std::endl;
            

            solver = new Solver(*ps);

        }

        void Sph3dComponent::simulate() {
            for (int i = 0; i < SPH3dPara::substep; i++) {
                ps->updateBlockInfo();
                solver->solve();
            }
        }

        GLuint Sph3dComponent::getRenderedTexture()
        {
            renderer->load(*ps);
            renderer->draw();
            return renderer->getRenderedTexture();
        }

        void Sph3dComponent::cameraMove(float x, float y) {
            Glb::Camera::getInstance().ProcessMove(glm::vec2(x, y));
        }

        void Sph3dComponent::cameraRotate(float x, float y) {
            Glb::Camera::getInstance().ProcessRotate(glm::vec2(x, y));
        }

        void Sph3dComponent::cameraScale(float w) {
            Glb::Camera::getInstance().ProcessScale(static_cast<float>(w));
        }
    }
}

