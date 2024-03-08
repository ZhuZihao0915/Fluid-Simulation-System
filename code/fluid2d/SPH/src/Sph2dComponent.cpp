#include "Sph2dComponent.h"

namespace FluidSimulation {
    namespace SPH2d {

        void Sph2dComponent::shutDown() {
            delete renderer, solver, ps;
            renderer = NULL;
            solver = NULL;
            ps = NULL;
        }

        void Sph2dComponent::init() {

            if (renderer != NULL || solver != NULL || ps != NULL) {
                delete renderer, solver, ps;
                renderer = NULL;
                solver = NULL;
                ps = NULL;
            }

            Glb::Timer::getInstance().clear();

            // initialize renderer
            renderer = new Renderer();
            renderer->Init();

            // initialize particle system
            // set the container's size
            ps = new ParticalSystem2d();
            ps->setContainerSize(glm::vec2(-1.0f, -1.0f), glm::vec2(2.0f, 2.0f));

            // add a fluid block
            ps->addFluidBlock(glm::vec2(-0.4, -0.4), glm::vec2(0.8, 0.8), glm::vec2(-0.0f, -0.0f), 0.02f);

            ps->updateBlockInfo();

            std::cout << "partical num = " << ps->mParticalInfos.size() << std::endl;

            // give the particle system to solver
            solver = new Solver(*ps);
        }

        void Sph2dComponent::simulate() {
            for (int i = 0; i < SPH2dPara::substep; i++) {
                ps->updateBlockInfo();
                solver->solve();
            }
        }

        GLuint Sph2dComponent::getRenderedTexture()
        {
            if (simulating) { Glb::Timer::getInstance().start(); }
            renderer->LoadVertexes(*ps);
            renderer->draw();
            if (simulating) { Glb::Timer::getInstance().recordTime("rendering"); }

            return renderer->GetRenderedTexture();
        }
    }
}

