#pragma once
#ifndef __SPH_2D_ITERATION_H__
#define __SPH_2D_ITERARION_H__

namespace FluidSimulation {
    namespace SPH2d {
        class Iteration {
        public:
            static Iteration& GetInstance() {
                static Iteration instance;
                return instance;
            }


            float GetDt() const { return dt; }
            void SetDt(float value) { dt = value; }

            int GetSubstep() const { return substep; }
            void SetSubstep(int value) { substep = value; }

        private:
            Iteration() {
                dt = 4e-4;
                substep = 5;
            }

            // 禁止拷贝构造和拷贝赋值
            Iteration(const Iteration&) = delete;
            Iteration& operator=(const Iteration&) = delete;

            float dt;         // 时间步长
            int substep;      // 子步数量（用于迭代求解）
        };
    }
}

#endif