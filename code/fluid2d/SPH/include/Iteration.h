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

            // ��ֹ��������Ϳ�����ֵ
            Iteration(const Iteration&) = delete;
            Iteration& operator=(const Iteration&) = delete;

            float dt;         // ʱ�䲽��
            int substep;      // �Ӳ����������ڵ�����⣩
        };
    }
}

#endif