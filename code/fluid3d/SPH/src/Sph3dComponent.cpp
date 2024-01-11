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


            ps.SetContainerSize(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.6, 0.6, 0.6));
            ps.AddFluidBlock(glm::vec3(0.05, 0.35, 0.25), glm::vec3(0.15, 0.15, 0.3), glm::vec3(0.0, 0.0, -3.0), 0.01 * 0.8);
            ps.AddFluidBlock(glm::vec3(0.35, 0.05, 0.25), glm::vec3(0.15, 0.15, 0.3), glm::vec3(0.0, 0.0, -3.0), 0.01 * 0.8);
            //ps.AddFluidBlock(glm::vec3(0.2, 0.2, 0.25), glm::vec3(0.2, 0.2, 0.3), glm::vec3(0.0, 0.0, 0.0), 0.01 * 0.8);

            // ���������򣬲��齨block�ṹ
            ps.updateBlockInfo();
            std::cout << "partical num = " << ps.mParticalInfos.size() << std::endl;
            
            // 
            renderer.Init();
            renderer.UploadUniforms(ps);

        }

        void Sph3dComponent::simulate() {
            
        }

        GLuint Sph3dComponent::getRenderedTexture()
        {

        }
    }
}

