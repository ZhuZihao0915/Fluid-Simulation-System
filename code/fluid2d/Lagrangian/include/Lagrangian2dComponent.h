#pragma once
#ifndef __LAGRANGIAN_2D_COMPONENT_H__
#define __LAGRANGIAN_2D_COMPONENT_H__

#include "Renderer.h"
#include "Solver.h"
#include "ParticalSystem2d.h"

#include "Component.h"
#include "Configure.h"


namespace FluidSimulation {
    namespace Lagrangian2d {
        class Lagrangian2dComponent : public Glb::Component {
        public:
            Renderer* renderer;
            Solver* solver;
            ParticalSystem2d* ps;

            Lagrangian2dComponent(char* description, int id) {
                this->description = description;
                this->id = id;
                renderer = NULL;
                solver = NULL;
                ps = NULL;
                is3D = false;
            }
            virtual void shutDown();
            virtual void init();
            virtual void simulate();
            virtual GLuint getRenderedTexture();
        };
    }
}


#endif