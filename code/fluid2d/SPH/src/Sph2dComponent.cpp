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

            // initialize renderer
            renderer = new Renderer();
            renderer->Init();

            // initialize particle system
            // set the container's size
            ps = new ParticalSystem2d(glm::vec2(Container::GetInstance().GetCornerX(), Container::GetInstance().GetCornerY()),
                glm::vec2(Container::GetInstance().GetSizeX(), Container::GetInstance().GetSizeY()));

            // add a fluid block
            ps->addFluidBlock(glm::vec2(-0.4, -0.4), glm::vec2(0.8, 0.8), glm::vec2(-0.0f, -0.0f), 0.014f);
            std::cout << "partical num = " << ps->positions.size() << std::endl;

            // give the particle system to solver
            solver = new Solver(*ps);
        }

        void Sph2dComponent::simulate() {
            for (int i = 0; i < Iteration::GetInstance().GetSubstep(); i++) {
                ps->searchNeighbors();
                solver->solve();
            }

            renderer->LoadVertexes(*ps);
            renderer->draw();
        }

        GLuint Sph2dComponent::getRenderedTexture()
        {
            return renderer->GetRenderedTexture();
        }
    }
}

