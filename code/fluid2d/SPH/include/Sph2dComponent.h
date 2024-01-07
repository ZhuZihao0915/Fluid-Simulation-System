#pragma once
#ifndef __SPH_2D_COMPONENT_H__
#define __SPH_2D_COMPONENT_H__

#include "Renderer.h"
#include "Solver.h"
#include "ParticalSystem2d.h"

#include "Component.h"
#include "Configure.h"

#include "Container.h"


namespace FluidSimulation {
    namespace SPH2d {
        class Sph2dComponent : public Glb::Component {
        public:
            Renderer* renderer;
            Solver* solver;
            ParticalSystem2d* ps;

            Sph2dComponent(char* description) {
                this->description = description;
                renderer = NULL;
                solver = NULL;
                ps = NULL;
            }
            virtual void shutDown();
            virtual void init();
            virtual void simulate();
            virtual GLuint getRenderedTexture();
        };
    }
}


#endif