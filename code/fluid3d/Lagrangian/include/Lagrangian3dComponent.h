#pragma once
#ifndef __LAGRANGIAN_3D_COMPONENT_H__
#define __LAGRANGIAN_3D_COMPONENT_H__

#include "Renderer.h"
#include "Solver.h"
#include "ParticalSystem3d.h"

#include "Component.h"
#include "Configure.h"


namespace FluidSimulation {
    namespace Lagrangian3d {
        class Lagrangian3dComponent : public Glb::Component {
        public:
            Renderer* renderer;
            Solver* solver;
            ParticalSystem3d* ps;

            Lagrangian3dComponent(char* description, int id) {
                this->description = description;
                this->id = id;
                renderer = NULL;
                solver = NULL;
                ps = NULL;
                is3D = true;
            }
            virtual void shutDown();
            virtual void init();
            virtual void simulate();
            virtual GLuint getRenderedTexture();
            virtual void cameraRotate(float x, float y);
            virtual void cameraMove(float x, float y);
            virtual void cameraScale(float w);
        };
    }
}


#endif