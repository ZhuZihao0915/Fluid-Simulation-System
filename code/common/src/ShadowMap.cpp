#include "ShadowMap.h"
#include <iostream>
#include "Global.h"

namespace Glb {
    ShadowMap::ShadowMap() {

    }

    ShadowMap::~ShadowMap() {

    }

    void ShadowMap::SetImageSize(int32_t w, int32_t h) {
        mWidth = w;
        mHeight = h;
    }

    void ShadowMap::SetLightInfo(PointLight& light) {
        mLightViewFront = glm::normalize(light.dir);
        mLightViewRight = glm::normalize(glm::cross(mLightViewFront, Glb::Z_AXIS));
        mLightViewUp = glm::normalize(glm::cross(mLightViewRight, mLightViewFront));
        mLightView = glm::lookAt(light.pos, light.pos + light.dir, mLightViewUp);
        float_t aspect = float_t(mWidth) / mHeight;
        mLightProjection = glm::perspective(glm::radians(light.fovy), aspect, SPH3D::zNear, SPH3D::zFar);
    }

    void ShadowMap::SetIor(float_t ior) {
        mIor = ior;
    }

    void ShadowMap::Init() {
        CreateBuffers(mWidth, mHeight);
        CreateShaders();
        InitIntrinsic();
    }

    void ShadowMap::Destroy() {
        delete mPointSpriteZValue;
    }

    void ShadowMap::Update(GLuint vaoParticals, int32_t particalNum, DepthFilter* depthFilter) {
        // 渲染深度图
        glViewport(0, 0, mWidth, mHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
        glClearColor(0.5f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mPointSpriteZValue->use();
        glBindVertexArray(vaoParticals);
        glDrawArrays(GL_POINTS, 0, particalNum);
        mPointSpriteZValue->unUse();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 平滑深度图
        mZBufferA = mTextureZBuffer;
        mZBufferB = mTextureTempZBuffer;
        depthFilter->Filter(mZBufferA, mZBufferB, glm::ivec2(mWidth, mHeight));
    }

    void ShadowMap::DrawCaustic(Camera* camera, GLuint vaoNull, const glm::mat4& model) {
        // 渲染焦散图
        glViewport(0, 0, mWidth, mHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, mFboCaustic);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_PROGRAM_POINT_SIZE);

        glBindImageTexture(0, mZBufferB, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
        glBindVertexArray(vaoNull);

        mCausticMap->use();
        mCausticMap->setMat4("model", model);
        for (int i = 0; i < Glb::ORIGIN_COLORS.size(); i++) {
            mCausticMap->setFloat("eta", 1.0 / (mIor + SPH3D::IOR_BIAS * float(i - 1)));
            mCausticMap->setVec3("photonColor", Glb::ORIGIN_COLORS[i] * SPH3D::CAUSTIC_FACTOR);
            glDrawArrays(GL_POINTS, 0, mWidth * mHeight);
        }
        mCausticMap->unUse();

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }

    GLuint ShadowMap::GetShadowMap() {
        return mZBufferB;
    }

    GLuint ShadowMap::GetCausticMap() {
        return mTextureCausticMap;
    }

    void ShadowMap::CreateShaders() {
        mPointSpriteZValue = new Glb::Shader();
        std::string pointSpriteZValueVertPath = shaderPath + "/ PointSprite.vert";
        std::string pointSpriteZValueGeomPath = shaderPath + "/PointSprite.geom";
        std::string pointSpriteZValueFragPath = shaderPath + "/PointSpriteZValue.frag";
        mPointSpriteZValue->buildFromFile(pointSpriteZValueVertPath, pointSpriteZValueFragPath, pointSpriteZValueGeomPath);
        mPointSpriteZValue->use();
        mPointSpriteZValue->setMat4("view", mLightView);
        mPointSpriteZValue->setMat4("projection", mLightProjection);
        mPointSpriteZValue->setFloat("particalRadius", SPH3D::particalDiameter);
        mPointSpriteZValue->setVec3("cameraUp", mLightViewUp);
        mPointSpriteZValue->setVec3("cameraRight", mLightViewRight);
        mPointSpriteZValue->setVec3("cameraFront", mLightViewFront);
        mPointSpriteZValue->setFloat("zFar", SPH3D::zFar);
        mPointSpriteZValue->setFloat("zNear", SPH3D::zNear);
        mPointSpriteZValue->unUse();

        mCausticMap = new Glb::Shader();
        std::string causticMapVertPath = shaderPath + "/CausticMap.vert";
        std::string causticMapFragPath = shaderPath + "/CausticMap.frag";
        mCausticMap->buildFromFile(causticMapVertPath, causticMapFragPath);
        mCausticMap->use();
        mCausticMap->setMat4("lightView", mLightView);
        mCausticMap->setMat4("lightProjection", mLightProjection);
        mCausticMap->setMat4("lightToWorld", glm::inverse(mLightView));
        mCausticMap->setMat4("lightToWorldRot", glm::mat4(glm::mat3(glm::inverse(mLightView))));
        mCausticMap->setInt("imageWidth", mWidth);
        mCausticMap->setInt("imageHeight", mHeight);
        mCausticMap->unUse();
    }

    void ShadowMap::CreateBuffers(int32_t w, int32_t h) {
        glGenTextures(1, &mTextureZBuffer);
        glBindTexture(GL_TEXTURE_2D, mTextureZBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &mTextureTempZBuffer);
        glBindTexture(GL_TEXTURE_2D, mTextureTempZBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        mZBufferA = mTextureZBuffer;
        mZBufferB = mTextureTempZBuffer;

        glGenTextures(1, &mTextureCausticMap);
        glBindTexture(GL_TEXTURE_2D, mTextureCausticMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glm::vec4 borderColor = glm::vec4(0.0f);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &borderColor.x);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenRenderbuffers(1, &mDepthStencil);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencil);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glGenFramebuffers(1, &mFbo);
        glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureZBuffer, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthStencil);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR: FluidShadowMap mFbo is not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(1, &mFboCaustic);
        glBindFramebuffer(GL_FRAMEBUFFER, mFboCaustic);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureCausticMap, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthStencil);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR: FluidShadowMap mFboCaustic is not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowMap::InitIntrinsic() {
        mCausticMap->use();
        mCausticMap->setVec4("lightIntrinsic", Glb::ProjToIntrinsic(mLightProjection, mWidth, mHeight));
        mCausticMap->unUse();
    }
}