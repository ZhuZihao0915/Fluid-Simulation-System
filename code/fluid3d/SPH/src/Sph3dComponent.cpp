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
            
            camera = new Glb::Camera();
            renderer = new Renderer(*camera);
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
            renderer->load(*ps);
            renderer->draw();
            
        }

        GLuint Sph3dComponent::getRenderedTexture()
        {
            return renderer->getRenderedTexture();
        }

        void Sph3dComponent::cameraMove(float x, float y) {
            renderer->mCamera.ProcessMove(glm::vec2(x, y));
        }

        void Sph3dComponent::cameraRotate(float x, float y) {
            renderer->mCamera.ProcessRotate(glm::vec2(x, y));
        }

        void Sph3dComponent::cameraScale(float w) {
            renderer->mCamera.ProcessScale(static_cast<float>(w));
        }
    }
}

