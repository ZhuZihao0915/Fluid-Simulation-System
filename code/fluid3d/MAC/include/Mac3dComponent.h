#pragma once
#ifndef __MAC_3D_COMPONENT_H__
#define __MAC_3D_COMPONENT_H__

#include "Renderer.h"
#include "Solver.h"
#include "MACGrid3d.h"

#include "Component.h"
#include "Configure.h"


namespace FluidSimulation {
    namespace MAC3d {
        class Mac3dComponent : public Glb::Component {
        public:
            Renderer* renderer;
            Solver* solver;
            MACGrid3d* grid;

            Mac3dComponent(char* description) {
                this->description = description;
                renderer = NULL;
                solver = NULL;
                grid = NULL;
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