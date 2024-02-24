#include "Sph2dComponent.h"

namespace FluidSimulation {
    namespace SPH2d {

        void Sph2dComponent::shutDown() {
            //renderer->destroy();
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

            // initialize renderer
            renderer = new Renderer();

            ps = new ParticalSystem2d(glm::vec2(-1.0f, -1.0f), glm::vec2(2.0f, 2.0f));
            ps->addFluidBlock(glm::vec2(-0.4, -0.4), glm::vec2(0.8, 0.8), glm::vec2(-0.0f, -0.0f), 0.014f);
            std::cout << "partical num = " << ps->positions.size() << std::endl;
            
            solver = new Solver(*ps);
        }

        void Sph2dComponent::simulate() {
            for (int i = 0; i < SPH2dPara::substep; i++) {
                ps->searchNeighbors();
                solver->solve();
            }

            renderer->draw(*ps);
        }

        GLuint Sph2dComponent::getRenderedTexture()
        {
            return renderer->GetRenderedTexture();
        }
    }
}

