#include "Renderer.h"

namespace FluidSimulation {
	namespace SPH3d {
		void Renderer::Init() {
            BuildShaders();

            GenerateFrameBuffers();
            GenerateBuffers();
            GenerateTextures();

            InitFilters();
            LoadSkyBox();
            CreateRenderAssets();
            MakeVertexArrays(); // 生成画粒子的vao

            glGenVertexArrays(1, &mVaoNull);
            glEnable(GL_MULTISAMPLE);
		}

        void Renderer::BluidShaders() {
            /*
            mComputeParticals = new Glb::ComputeShader("ComputeParticals");
            std::vector<std::string> computeShaderpaths = {
                std::string("../code/Fluid3d/Shaders/ComputeParticals.comp"),
            };
            mComputeParticals->buildFromFiles(computeShaderpaths);
            mComputeParticals->use();
            glUniform1i(glGetUniformLocation(mComputeParticals->getId(), "kernelBuffer"), 1);
            mComputeParticals->unUse();
            */

            mScreenQuad = new Glb::Shader();
            std::string screenQuadVertPath = shaderPath + "/ScreenQuad.vert";
            std::string screenQuadFragPath = shaderPath + "/ScreenQuad.frag";
            mScreenQuad->buildFromFile(screenQuadVertPath, screenQuadFragPath);
            mScreenQuad->use();
            glUniform1i(glGetUniformLocation(mScreenQuad->getId(), "tex"), 0);
            mScreenQuad->unUse();

            mDrawColor3d = new Glb::Shader();
            std::string drawColorVertPath = shaderPath + "/DrawColor3d.vert";
            std::string drawColorFragPath = shaderPath + "/DrawColor3d.frag";
            mDrawColor3d->buildFromFile(drawColorVertPath, drawColorFragPath);

            mPointSpriteZValue = new Glb::Shader();
            std::string pointSpriteZValueVertPath = shaderPath + "/PointSprite.vert";
            std::string pointSpriteZValueGeomPath = shaderPath + "/PointSprite.geom";
            std::string pointSpriteZValueFragPath = shaderPath + "/PointSpriteZValue.frag";
            mPointSpriteZValue->buildFromFile(pointSpriteZValueVertPath, pointSpriteZValueFragPath, pointSpriteZValueGeomPath);
            mPointSpriteZValue->use();
            mPointSpriteZValue->setFloat("zFar", Para3d::zFar);
            mPointSpriteZValue->setFloat("zNear", Para3d::zNear);
            mPointSpriteZValue->unUse();

            mPointSpriteThickness = new Glb::Shader();
            std::string pointSpriteThicknessVertPath = shaderPath + "/PointSprite.vert";
            std::string pointSpriteThicknessGeomPath = shaderPath + "/PointSprite.geom";
            std::string pointSpriteThicknessFragPath = shaderPath + "/PointSpriteThickness.frag";
            mPointSpriteThickness->buildFromFile(pointSpriteThicknessVertPath, pointSpriteThicknessFragPath, pointSpriteThicknessGeomPath);

            mDrawFluidColor = new Glb::Shader();
            std::string drawFluidColorVertPath = shaderPath + "/DrawFluidColor.vert";
            std::string drawFluidColorFragPath = shaderPath + "/DrawFluidColor.frag";
            mDrawFluidColor->buildFromFile(drawFluidColorVertPath, drawFluidColorFragPath);
            mDrawFluidColor->use();
            mDrawFluidColor->setFloat("zFar", Para3d::zFar);
            mDrawFluidColor->setFloat("zNear", Para3d::zNear);
            mDrawFluidColor->setFloat("eta", 1.0 / Para3d::IOR);
            mDrawFluidColor->setVec3("f0", Para3d::F0);
            mDrawFluidColor->setVec4("cameraIntrinsic", Glb::ProjToIntrinsic(mCamera.GetProjection(), mWindowWidth, mWindowHeight));
            mDrawFluidColor->setVec3("fluidColor", Para3d::FLUID_COLOR);
            mDrawFluidColor->setVec3("shadowColor", Para3d::SHADOW_COLOR);
            mDrawFluidColor->setFloat("thicknessFactor", Para3d::THICKNESS_FACTOR);
            mDrawFluidColor->unUse();

            mDrawModel = new Glb::Shader();
            std::string drawModelVertPath = shaderPath + "/DrawModel.vert";
            std::string drawModelFragPath = shaderPath + "/DrawModel.frag";
            mDrawModel->buildFromFile(drawModelVertPath, drawModelFragPath);
        }

        void Renderer::GenerateFrameBuffers() {
            // depth framebuffer
            glGenFramebuffers(1, &mFboDepth);

            glGenTextures(1, &mTexZBuffer);
            glBindTexture(GL_TEXTURE_2D, mTexZBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWindowWidth, mWindowHeight, 0, GL_RED, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);

            glGenRenderbuffers(1, &mRboDepthBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, mRboDepthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWindowWidth, mWindowHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, mFboDepth);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexZBuffer, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRboDepthBuffer);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "ERROR: mFboDepth is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // thickness framebuffer
            glGenFramebuffers(1, &mFboThickness);

            glGenTextures(1, &mTexThicknessBuffer);
            glBindTexture(GL_TEXTURE_2D, mTexThicknessBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWindowWidth, mWindowHeight, 0, GL_RED, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, mFboThickness);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexThicknessBuffer, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRboDepthBuffer);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "ERROR: mFboThickness is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Renderer::GenerateBuffers() {
            glGenBuffers(1, &mCoordVertBuffer);     // coord vbo
            glGenBuffers(1, &mBufferParticals);     // ssbo
            glGenBuffers(1, &mBufferBlocks);
            glGenBuffers(1, &mBufferFloor);
        }

        void Renderer::GenerateTextures() {
            // 测试用的纹理
            glGenTextures(1, &mTestTexture);
            glBindTexture(GL_TEXTURE_2D, mTestTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 100, 100, 0, GL_RGBA, GL_FLOAT, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);

            // 核函数纹理
            glGenTextures(1, &mTexKernelBuffer);
            glBindTexture(GL_TEXTURE_1D, mTexKernelBuffer);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_1D, 0);

            // 模糊Z后的坐标图
            glGenTextures(1, &mTexZBlurTempBuffer);
            glBindTexture(GL_TEXTURE_2D, mTexZBlurTempBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWindowWidth, mWindowHeight, 0, GL_RED, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);

        }

        void Renderer::InitFilters() {
            mDepthFilter = new DepthFilter();
            mDepthFilter->Create(8.0, 0.025);
        }

        void Renderer::LoadSkyBox() {
            mSkyBox = new SkyBox();
            mSkyBox->Create();
            std::vector<std::string> paths
            {
                "../../../../code/resources/skybox/right.jpg",
                "../../../../code/resources/skybox/left.jpg",
                "../../../../code/resources/skybox/top.jpg",
                "../../../../code/resources/skybox/bottom.jpg",
                "../../../../code/resources/skybox/front.jpg",
                "../../../../code/resources/skybox/back.jpg"
            };
            mSkyBox->LoadImages(paths);
            mSkyBox->BuildShader();
        }

        void Renderer::CreateRenderAssets() {
            // 材质
            mSlabWhite = new Material();
            mSlabWhite->Create();
            std::string albedoPath = "../resources/SlabWhite/TexturesCom_Marble_SlabWhite_1K_albedo.png";
            std::string roughnessPath = "../resources/SlabWhite/TexturesCom_Marble_SlabWhite_1K_roughness.png";
            mSlabWhite->LoadTextures(albedoPath, roughnessPath);

            // 灯光
            mLight.pos = glm::vec3(-0.8, -0.8, 2.0);
            mLight.dir = glm::vec3(0.5, 0.5, -1.0);
            mLight.aspect = 1.0f;
            mLight.fovy = 30.0;

            // 阴影贴图
            mShadowMap = new FluidShadowMap();
            mShadowMap->SetImageSize(1000, 1000);
            mShadowMap->SetLightInfo(mLight);
            mShadowMap->SetIor(Para3d::IOR);
            mShadowMap->Init();
        }

        void Renderer::MakeVertexArrays() {
            glGenVertexArrays(1, &mVaoParticals);
            glBindVertexArray(mVaoParticals);
            glBindBuffer(GL_ARRAY_BUFFER, mBufferParticals);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticalInfo3d), (void*)offsetof(ParticalInfo3d, position));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticalInfo3d), (void*)offsetof(ParticalInfo3d, density));
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);

            glGenVertexArrays(1, &mVaoCoord);
            glBindVertexArray(mVaoCoord);
            glBindBuffer(GL_ARRAY_BUFFER, mCoordVertBuffer);
            glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), vertexes, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);

            glGenVertexArrays(1, &mVaoFloor);
            glBindVertexArray(mVaoFloor);
            glBindBuffer(GL_ARRAY_BUFFER, mBufferFloor);
            glBufferData(GL_ARRAY_BUFFER, floorVertices.size() * sizeof(float_t), floorVertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float_t)));
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }

	}
}