#include "Lagrangian2dComponent.h"

namespace FluidSimulation
{
    namespace Lagrangian2d
    {

        void Lagrangian2dComponent::shutDown()
        {
            delete renderer, solver, ps;
            renderer = NULL;
            solver = NULL;
            ps = NULL;
        }

        void Lagrangian2dComponent::init()
        {

            if (renderer != NULL || solver != NULL || ps != NULL)
            {
                delete renderer, solver, ps;
                renderer = NULL;
                solver = NULL;
                ps = NULL;
            }

            Glb::Timer::getInstance().clear();

            // initialize renderer
            renderer = new Renderer();
            renderer->init();

            // initialize particle system
            // set the container's size
            ps = new ParticleSystem2d();
            ps->setContainerSize(glm::vec2(-2.0f, -2.0f), glm::vec2(4.0f, 4.0f));

            // add a fluid block
            ps->addFluidBlock(glm::vec2(-0.8, -0.8), glm::vec2(1.6, 1.6), glm::vec2(-0.0f, -0.0f), 0.02f);

            ps->updateBlockInfo();

            std::cout << "particle num = " << ps->mParticleInfos.size() << std::endl;

            solver = new Solver(*ps);
        }

        void Lagrangian2dComponent::simulate()
        {
            for (int i = 0; i < Lagrangian2dPara::substep; i++)
            {
                ps->updateBlockInfo();
                solver->solve();
            }
        }

        GLuint Lagrangian2dComponent::getRenderedTexture()
        {
            if (simulating)
            {
                Glb::Timer::getInstance().start();
            }
            renderer->LoadVertexes(*ps);
            renderer->draw();
            if (simulating)
            {
                Glb::Timer::getInstance().recordTime("rendering");
            }

            return renderer->GetRenderedTexture();
        }
    }
}
