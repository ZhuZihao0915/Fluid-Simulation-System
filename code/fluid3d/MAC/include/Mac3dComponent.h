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

            Mac3dComponent(char* description, int id) {
                this->description = description;
                this->id = id;
                renderer = NULL;
                solver = NULL;
                grid = NULL;
                camera = NULL;
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