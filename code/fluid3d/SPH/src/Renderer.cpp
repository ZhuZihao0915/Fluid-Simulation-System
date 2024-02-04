#include "fluid3d/SPH/include/Renderer.h"

namespace FluidSimulation {
	namespace SPH3d {

        Renderer::Renderer() {

        }

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

        void Renderer::BuildShaders() {
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
            
            mDrawColor3d = new Glb::Shader();
            std::string drawColorVertPath = shaderPath + "/DrawColor3d.vert";
            std::string drawColorFragPath = shaderPath + "/DrawColor3d.frag";
            //std::string drawColorGeomPath = shaderPath + "/DrawColor3d.geom";
            mDrawColor3d->buildFromFile(drawColorVertPath, drawColorFragPath);// , drawColorGeomPath);

            /*mScreenQuad = new Glb::Shader();
            std::string screenQuadVertPath = shaderPath + "/ScreenQuad.vert";
            std::string screenQuadFragPath = shaderPath + "/ScreenQuad.frag";
            mScreenQuad->buildFromFile(screenQuadVertPath, screenQuadFragPath);
            mScreenQuad->use();
            glUniform1i(glGetUniformLocation(mScreenQuad->getId(), "tex"), 0);
            mScreenQuad->unUse();


            mPointSpriteZValue = new Glb::Shader();
            std::string pointSpriteZValueVertPath = shaderPath + "/PointSprite.vert";
            std::string pointSpriteZValueGeomPath = shaderPath + "/PointSprite.geom";
            std::string pointSpriteZValueFragPath = shaderPath + "/PointSpriteZValue.frag";
            mPointSpriteZValue->buildFromFile(pointSpriteZValueVertPath, pointSpriteZValueFragPath, pointSpriteZValueGeomPath);
            mPointSpriteZValue->use();
            mPointSpriteZValue->setFloat("zFar", SPH3D::zFar);
            mPointSpriteZValue->setFloat("zNear", SPH3D::zNear);
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
            mDrawFluidColor->setFloat("zFar", SPH3D::zFar);
            mDrawFluidColor->setFloat("zNear", SPH3D::zNear);
            mDrawFluidColor->setFloat("eta", 1.0 / SPH3D::IOR);
            mDrawFluidColor->setVec3("f0", SPH3D::F0);
            mDrawFluidColor->setVec4("cameraIntrinsic", Glb::ProjToIntrinsic(mCamera.GetProjection(), mWindowWidth, mWindowHeight));
            mDrawFluidColor->setVec3("fluidColor", SPH3D::FLUID_COLOR);
            mDrawFluidColor->setVec3("shadowColor", SPH3D::SHADOW_COLOR);
            mDrawFluidColor->setFloat("thicknessFactor", SPH3D::THICKNESS_FACTOR);
            mDrawFluidColor->unUse();

            mDrawModel = new Glb::Shader();
            std::string drawModelVertPath = shaderPath + "/DrawModel.vert";
            std::string drawModelFragPath = shaderPath + "/DrawModel.frag";
            mDrawModel->buildFromFile(drawModelVertPath, drawModelFragPath);*/
        }

        void Renderer::GenerateFrameBuffers() {
            // NEW!!!
            // generate frame buffer object
            glGenFramebuffers(1, &fbo);
            // make it active
            // start fbo
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            // generate textures
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Texture2D绑定到FBO
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

            // generate render buffer object (RBO)
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // RBO绑定到FBO
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "ERROR: SDF Framebuffer is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // END!!!

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
            /*mDepthFilter = new Glb::DepthFilter();
            mDepthFilter->Create(8.0, 0.025);*/
        }

        void Renderer::LoadSkyBox() {
            mSkyBox = new Glb::SkyBox();
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
            //mSlabWhite = new Glb::Material();
            //mSlabWhite->Create();
            //std::string albedoPath = "../../../../code/resources/SlabWhite/TexturesCom_Marble_SlabWhite_1K_albedo.png";
            //std::string roughnessPath = "../../../../code/resources/SlabWhite/TexturesCom_Marble_SlabWhite_1K_roughness.png";
            //mSlabWhite->LoadTextures(albedoPath, roughnessPath);

            //// 灯光
            //mLight.pos = glm::vec3(-0.8, -0.8, 2.0);
            //mLight.dir = glm::vec3(0.5, 0.5, -1.0);
            //mLight.aspect = 1.0f;
            //mLight.fovy = 30.0;

            //// 阴影贴图
            //mShadowMap = new Glb::FluidShadowMap();
            //mShadowMap->SetImageSize(1000, 1000);
            //mShadowMap->SetLightInfo(mLight);
            //mShadowMap->SetIor(SPH3D::IOR);
            //mShadowMap->Init();
        }

        // 生成需要渲染的object的VAO
        void Renderer::MakeVertexArrays() {

            // 粒子
            glGenVertexArrays(1, &mVaoParticals);
            glBindVertexArray(mVaoParticals);
            glBindBuffer(GL_ARRAY_BUFFER, mBufferParticals);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticalInfo3d), (void*)offsetof(ParticalInfo3d, position));
            glEnableVertexAttribArray(0);   // location = 0
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticalInfo3d), (void*)offsetof(ParticalInfo3d, density));
            glEnableVertexAttribArray(1);   // location = 1
            glBindVertexArray(0);

            // 坐标轴
            glGenVertexArrays(1, &mVaoCoord);
            glBindVertexArray(mVaoCoord);
            glBindBuffer(GL_ARRAY_BUFFER, mCoordVertBuffer);
            glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), SPH3D::vertexes, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);

            // 底板
            glGenVertexArrays(1, &mVaoFloor);
            glBindVertexArray(mVaoFloor);
            glBindBuffer(GL_ARRAY_BUFFER, mBufferFloor);
            glBufferData(GL_ARRAY_BUFFER, SPH3D::floorVertices.size() * sizeof(float_t), SPH3D::floorVertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float_t)));
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        void Renderer::load(ParticalSystem3d& ps) {
            // 申请装粒子信息的buffer
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferParticals);
            glBufferData(GL_SHADER_STORAGE_BUFFER, ps.mParticalInfos.size() * sizeof(ParticalInfo3d), ps.mParticalInfos.data(), GL_DYNAMIC_COPY);

            // 申请block区间buffer
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferBlocks);
            glBufferData(GL_SHADER_STORAGE_BUFFER, ps.mBlockExtens.size() * sizeof(glm::uvec2), ps.mBlockExtens.data(), GL_DYNAMIC_COPY);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            mParticalNum = ps.mParticalInfos.size();
        }

        void Renderer::draw() {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_PROGRAM_POINT_SIZE);

            mDrawColor3d->use();
            mDrawColor3d->setMat4("view", mCamera.GetView());
            mDrawColor3d->setMat4("projection", mCamera.GetProjection());

            glBindVertexArray(mVaoCoord);
            glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, SPH3D::indices);
            glBindVertexArray(mVaoParticals);
            //glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mParticalNum);
            glDrawArrays(GL_POINTS, 0, mParticalNum);
            mSkyBox->Draw(mWindow, mVaoNull, mCamera.GetView(), mCamera.GetProjection());
            mDrawColor3d->unUse();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint Renderer::getRenderedTexture() {
            return textureID;
        }
	}
}