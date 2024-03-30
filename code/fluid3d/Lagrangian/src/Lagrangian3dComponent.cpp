#include "Lagrangian3dComponent.h"

namespace FluidSimulation
{
    namespace Lagrangian3d
    {

        void Lagrangian3dComponent::shutDown()
        {
            delete renderer, solver, ps;
            renderer = NULL;
            solver = NULL;
            ps = NULL;
        }

        void Lagrangian3dComponent::init()
        {
            if (renderer != NULL || solver != NULL || ps != NULL)
            {
                shutDown();
            }
            Glb::Timer::getInstance().clear();

            renderer = new Renderer();
            renderer->init();

            ps = new ParticleSystem3d();
            ps->setContainerSize(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1, 1, 1));
            ps->addFluidBlock(glm::vec3(0.05, 0.05, 0.3 ), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.0, 0.0, -1.0), 0.02);
            ps->addFluidBlock(glm::vec3(0.45, 0.45, 0.3), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.0, 0.0, -1.0), 0.02);

            ps->updateBlockInfo();
            Glb::Logger::getInstance().addLog("2d Particle System initialized. particle num: " + std::to_string(ps->particles.size()));

            solver = new Solver(*ps);
        }

        void Lagrangian3dComponent::simulate()
        {
            for (int i = 0; i < Lagrangian3dPara::substep; i++)
            {
                ps->updateBlockInfo();
                solver->solve();
            }
        }

        GLuint Lagrangian3dComponent::getRenderedTexture()
        {
            if (simulating)
            {
                Glb::Timer::getInstance().start();
            }
            renderer->draw(*ps);
            if (simulating)
            {
                Glb::Timer::getInstance().recordTime("rendering");
            }
            return renderer->getRenderedTexture();
        }
    }
}
