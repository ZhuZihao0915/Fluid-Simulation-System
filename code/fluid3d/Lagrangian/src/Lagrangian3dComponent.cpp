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
                delete renderer, solver, ps;
                renderer = NULL;
                solver = NULL;
                ps = NULL;
            }
            Glb::Timer::getInstance().clear();

            renderer = new Renderer();
            renderer->init();

            ps = new ParticleSystem3d();
            ps->setContainerSize(glm::vec3(0.0, 0.0, 0.0), glm::vec3(2, 2, 2));
            ps->addFluidBlock(glm::vec3(0.05 * 2, 0.05 * 2, 0.4 * 2), glm::vec3(0.4 * 2, 0.4 * 2, 0.4 * 2), glm::vec3(0.0, 0.0, -1.0), 0.02);
            ps->addFluidBlock(glm::vec3(0.45 * 2, 0.45 * 2, 0.4 * 2), glm::vec3(0.4 * 2, 0.4 * 2, 0.4 * 2), glm::vec3(0.0, 0.0, -1.0), 0.02);

            ps->updateBlockInfo();
            std::cout << "particle num = " << ps->mParticleInfos.size() << std::endl;

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
            renderer->load(*ps);
            renderer->draw();
            if (simulating)
            {
                Glb::Timer::getInstance().recordTime("rendering");
            }

            return renderer->getRenderedTexture();
        }

        void Lagrangian3dComponent::cameraMove(float x, float y)
        {
            Glb::Camera::getInstance().ProcessMove(glm::vec2(x, y));
        }

        void Lagrangian3dComponent::cameraRotate(float x, float y)
        {
            Glb::Camera::getInstance().ProcessRotate(glm::vec2(x, y));
        }

        void Lagrangian3dComponent::cameraScale(float w)
        {
            Glb::Camera::getInstance().ProcessScale(static_cast<float>(w));
        }
    }
}
