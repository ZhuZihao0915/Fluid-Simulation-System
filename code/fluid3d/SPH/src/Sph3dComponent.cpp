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

            // 
            renderer = new Renderer;
            renderer->Init();

            ps = new ParticalSystem3d();
            ps->setContainerSize(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.6, 0.6, 0.6));
            ps->addFluidBlock(glm::vec3(0.05, 0.35, 0.25), glm::vec3(0.15, 0.15, 0.3), glm::vec3(0.0, 0.0, -3.0), 0.01 * 0.8, 0.01 * 0.8, 0.01 * 0.8);// 0.01 * 0.8);
            ps->addFluidBlock(glm::vec3(0.35, 0.05, 0.25), glm::vec3(0.15, 0.15, 0.3), glm::vec3(0.0, 0.0, -3.0), 0.01 * 0.8, 0.01 * 0.8, 0.01 * 0.8);
            
            //ps->addFluidBlock(glm::vec3(0.01, 0.01, 0.5), glm::vec3(0.4, 0.4, 0.4), glm::vec3(-0.2, -0.2, 0.0), 0.03, 0.03, 0.01);// 0.01 * 0.8);
            //ps->addFluidBlock(glm::vec3(0.59, 0.59, 0.5), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.2, 0.2, 0.0), 0.03, 0.03, 0.01);
            // ps.sddFluidBlock(glm::vec3(0.2, 0.2, 0.25), glm::vec3(0.2, 0.2, 0.3), glm::vec3(0.0, 0.0, 0.0), 0.01 * 0.8);

            // 对粒子排序，并组建block结构
            ps->updateBlockInfo();
            std::cout << "partical num = " << ps->mParticalInfos.size() << std::endl;
            
            solver = new Solver(*ps);

        }

        void Sph3dComponent::simulate() {
            for (int i = 0; i < SPH3D::substep; i++) {
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

