#pragma once
#ifndef __SPH_2D_CONTAINER_H__
#define __SPH_2D_CONTAINER_H__


namespace FluidSimulation {
    namespace SPH2d {
        class Container {
        public:
            static Container& GetInstance() {
                static Container instance;
                return instance;
            }

            float GetCornerX() const { return corner_x; };
            float GetCornerY() const { return corner_y; };
            float GetSizeX() const { return size_x; };
            float GetSizeY() const { return size_y; };
            void SetCornerX(float x) { corner_x = x; };
            void SetCornerY(float y) { corner_y = y; };
            void SetSizeX(float x) { size_x = x; };
            void SetSizeY(float y) { size_y = y; };


        private:
            Container() {
                corner_x = -1.0f;
                corner_y = -1.0f;
                size_x = 2.0f;
                size_y = 2.0f;
            }

            Container(const Container&) = delete;
            Container& operator=(const Container&) = delete;

            float corner_x; // -1.0 ~ 1.0
            float corner_y;
            float size_x;   // 0.0 ~ 2.0
            float size_y;
        };
    }
}

#endif