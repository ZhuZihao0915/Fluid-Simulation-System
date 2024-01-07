#pragma once
#ifndef __SPH_3D_COMPONENT_H__
#define __SPH_3D_COMPONENT_H__

#include "Renderer.h"
#include "Solver.h"
#include "ParticalSystem3d.h"

#include "Component.h"
#include "Configure.h"


namespace FluidSimulation {
    namespace SPH3d {
        class Sph3dComponent : public Glb::Component {
        public:
            Renderer* renderer;
            Solver* solver;
            ParticalSystem3d* ps;

            Sph3dComponent(char* description) {
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