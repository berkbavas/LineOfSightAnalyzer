#include "TerrainRenderer.h"

#include "Core/CameraManager.h"
#include "Renderer/LineOfSightRenderer.h"

#include <QtImgui.h>
#include <imgui.h>

void LineOfSightAnalyzer::TerrainRenderer::Init()
{
    initializeOpenGLFunctions();

    mFramebufferFormat = QOpenGLFramebufferObjectFormat();
    mFramebufferFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    mFramebufferFormat.setSamples(0);
    mFramebufferFormat.setTextureTarget(GL_TEXTURE_2D);

    mTerrainShader = new Shader("Terrain Shader");
    mTerrainShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
    mTerrainShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tcs");
    mTerrainShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tes");
    mTerrainShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Terrain.frag");
    mTerrainShader->Initialize();

    mTextureLoader = new TextureLoader;

    Texture texture = mTextureLoader->LoadTexture2D("Resources/HeightMaps/01.png");
    texture.name = "heightMap";
    texture.unit = 0;

    mTerrain = new Terrain(texture.width, texture.height, 32);
    mTerrain->SetTexture(texture);
    mTerrain->Construct();
}

void LineOfSightAnalyzer::TerrainRenderer::Render(float)
{
    mActiveCamera = mCameraManager->GetActiveCamera();

    mFramebuffer->bind();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mTerrainShader->Bind();
    mTerrainShader->SetUniformValue("model", mTerrain->GetTransformation());
    mTerrainShader->SetUniformValue("view", mActiveCamera->GetViewMatrix());
    mTerrainShader->SetUniformValue("projection", mActiveCamera->GetProjectionMatrix());
    mTerrainShader->SetUniformValue("minElevation", mTerrain->GetMinElevation());
    mTerrainShader->SetUniformValue("maxElevation", mTerrain->GetMaxElevation());
    mTerrainShader->SetUniformValue("observerPosition", mLineOfSightAnalyzer->GetObservers().at(0)->GetPosition());
    mTerrainShader->SetUniformValue("farPlane", mLineOfSightAnalyzer->GetObservers().at(0)->GetZFar());
    mTerrainShader->SetUniformValue("bias", mBias);
    mTerrainShader->ActivateTexture(mTerrain->GetTexture());
    mTerrainShader->ActivateTexture(mLineOfSightAnalyzer->GetDepthMap());
    mTerrain->Render();
    mTerrainShader->Release();

    QOpenGLFramebufferObject::bindDefault();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QOpenGLFramebufferObject::blitFramebuffer(nullptr, //
                                              QRect(0, 0, mWidth, mHeight),
                                              mFramebuffer,
                                              QRect(0, 0, mFramebuffer->width(), mFramebuffer->height()),
                                              GL_COLOR_BUFFER_BIT,
                                              GL_NEAREST,
                                              0,
                                              0);
}

void LineOfSightAnalyzer::TerrainRenderer::DrawGui()
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Elevation");
    ImGui::SliderFloat("Min Elevation##RenderSettings", &mTerrain->GetMinElevation_NonConst(), 0, 100);
    ImGui::SliderFloat("Max Elevation##RenderSettings", &mTerrain->GetMaxElevation_NonConst(), mTerrain->GetMinElevation(), 1000);
    ImGui::SliderFloat("Bias##RenderSettings", &mBias, 0.0f, 0.1f);

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Info");
    ImGui::Text("Mouse World Position: (%.1f, %.1f, %.1f)", mMouseWorldPosition[0], mMouseWorldPosition[1], mMouseWorldPosition[2]);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void LineOfSightAnalyzer::TerrainRenderer::Resize(int w, int h)
{
    mWidth = w;
    mHeight = h;

    DeleteFramebuffer();
    CreateFramebuffer();
}

void LineOfSightAnalyzer::TerrainRenderer::onMouseMoved(QMouseEvent* event)
{
    mFramebuffer->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(event->pos().x() * mDevicePixelRatio, mFramebuffer->height() - event->pos().y() * mDevicePixelRatio, 1, 1, GL_RGB, GL_FLOAT, &mMouseWorldPosition);
    mFramebuffer->release();
}

void LineOfSightAnalyzer::TerrainRenderer::DeleteFramebuffer()
{
    if (mFramebuffer)
    {
        delete mFramebuffer;
        mFramebuffer = nullptr;
    }
}

void LineOfSightAnalyzer::TerrainRenderer::CreateFramebuffer()
{
    mFramebuffer = new QOpenGLFramebufferObject(mWidth, mHeight, mFramebufferFormat);
    mFramebuffer->addColorAttachment(mWidth, mHeight, GL_RGB32F);
    mFramebuffer->bind();
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    mFramebuffer->release();
}
