#pragma once
#ifndef __SPH_2D_FLUID_H__
#define __SPH_2D_FLUID_H__

namespace FluidSimulation {
    namespace SPH2d {
        class Fluid {
        public:

            static Fluid& GetInstance() {
                static Fluid instance;
                return instance;
            }

            float GetGravity() const { return gravity; }
            void SetGravity(float value) { gravity = value; }

            float GetDensity() const { return density; }
            void SetDensity(float value) { density = value; }

            float GetStiffness() const { return stiffness; }
            void SetStiffness(float value) { stiffness = value; }

            float GetExponent() const { return exponent; }
            void SetExponent(float value) { exponent = value; }

            float GetViscosity() const { return viscosity; }
            void SetViscosity(float value) { viscosity = value; }

        private:

            Fluid() {

                gravity = 9.8f;
                density = 1000.0f;
                stiffness = 70.0f;
                exponent = 7.0f;
                viscosity = 0.03f;
            }


            Fluid(const Fluid&) = delete;
            Fluid& operator=(const Fluid&) = delete;


            float gravity;    // 重力
            float density;    // 流体密度(normal)
            float stiffness;  // 弹性/刚度系数
            float exponent;   // 流体模拟中的指数
            float viscosity;  // 粘性
        };
    }
}


#endif