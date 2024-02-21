#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Glb {
    class Camera {
    public:
        Camera();
        ~Camera();

        void ProcessMove(glm::vec2 offset);                //处理相机移动
        void ProcessRotate(glm::vec2 offset);              //处理相机旋转
        void ProcessScale(float offset);                 //处理相机缩放

        glm::mat4 GetView();                             //获取视图矩阵
        glm::mat4 GetProjection();                        //获取投影矩阵
        glm::vec3 GetUp();                               //获取相机上方向量
        glm::vec3 GetRight();                             //获取相机右方向量
        glm::vec3 GetFront();                             //获取相机前方向量
        glm::vec3 GetPosition();

        void UpdateView();                                //更新视图矩阵

    public:
        float_t mYaw;                                     //相机偏航角
        float_t mPitch;                                   //相机俯仰角

        float_t mSensitiveYaw = 0.1;                    //相机偏航角灵敏度
        float_t mSensitivePitch = 0.1;                  //相机俯仰角灵敏度
        float_t mSensitiveX = 0.001;                     //相机移动灵敏度
        float_t mSensitiveY = 0.001;                     //相机移动灵敏度
        float_t mSensitiveFront = 0.05;                  //相机缩放灵敏度

        glm::vec3 mPosition;                             //相机位置
        //glm::vec3 mTargetPoint;                          //相机目标点
        float mTargetDistance;                           //相机与目标点的距离
        glm::vec3 mRight;                                 //相机右方向向量
        glm::vec3 mUp;                                   //相机上方向向量
        glm::vec3 mFront;                                 //相机前方向向量
        glm::vec3 mWorldUp;                              //世界上的方向向量


        float aspect;
        float nearPlane;
        float farPlane;
        float fovyDeg;

    };
}


#endif