#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Glb {
    class Camera {
    public:
        Camera();
        ~Camera();

        void ProcessMove(glm::vec2 offset);                //��������ƶ�
        void ProcessRotate(glm::vec2 offset);              //���������ת
        void ProcessScale(float offset);                 //�����������
        void SetPerspective(float aspect = 1.0f, float nearPlane = 0.1f, float mFarPlane = 100.0f, float fovyDeg = 60.0f);
        //����͸��ͶӰ

        glm::mat4 GetView();                             //��ȡ��ͼ����
        glm::mat4 GetProjection();                        //��ȡͶӰ����
        glm::vec3 GetUp();                               //��ȡ����Ϸ�����
        glm::vec3 GetRight();                             //��ȡ����ҷ�����
        glm::vec3 GetFront();                             //��ȡ���ǰ������

    private:
        void UpdateView();                                //������ͼ����

    private:
        float_t mYaw;                                     //���ƫ����
        float_t mPitch;                                   //���������
        float_t mSensitiveYaw = 0.1;                    //���ƫ����������
        float_t mSensitivePitch = 0.1;                  //���������������
        float_t mSensitiveX = 0.001;                     //����ƶ�������
        float_t mSensitiveY = 0.001;                     //����ƶ�������
        float_t mSensitiveFront = 0.05;                  //�������������

        glm::vec3 mPosition;                             //���λ��
        glm::vec3 mTargetPoint;                          //���Ŀ���
        float mTargetDistance;                           //�����Ŀ���ľ���
        glm::vec3 mRight;                                 //����ҷ�������
        glm::vec3 mUp;                                   //����Ϸ�������
        glm::vec3 mFront;                                 //���ǰ��������
        glm::vec3 mWorldUp;                              //�����ϵķ�������
        glm::mat4 mView;                                 //��ͼ����

        glm::mat4 mProjection;                           //ͶӰ����

    };
}


#endif