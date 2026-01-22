#include "LineOfSightRenderer.h"

#include "Core/Constants.h"
#include "Node/Camera/FreeCamera.h"
#include "Renderer/TerrainRenderer.h"

#include <QtImgui.h>
#include <imgui.h>

void LineOfSightAnalyzer::LineOfSightRenderer::Init()
{
    initializeOpenGLFunctions();

    mObserverShader = new Shader("Observer Shader");
    mObserverShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
    mObserverShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tcs");
    mObserverShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tes");
    mObserverShader->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/Observer.geom");
    mObserverShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Observer.frag");
    mObserverShader->Initialize();

    // Line of sight stuff
    {
        for (int i = 0; i < 6; i++)
        {
            mObservers << new FreeCamera;
            mObservers[i]->SetPosition(QVector3D(0, 100, 0));
            mObservers[i]->Resize(OBSERVER_FBO_WIDTH, OBSERVER_FBO_HEIGHT);
            mObservers[i]->SetVerticalFov(90.0f);
            mObservers[i]->SetZNear(mMinLosDistance);
            mObservers[i]->SetZFar(mMaxLosDistance);
        }
        const auto rollFix = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), 180);

        mObservers[0]->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), -90) * rollFix);
        mObservers[1]->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 90) * rollFix);
        mObservers[2]->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90));
        mObservers[3]->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
        mObservers[4]->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180) * rollFix);
        mObservers[5]->SetRotation(rollFix);

        glGenFramebuffers(1, &mFramebuffer);
        glGenTextures(1, &mDepthMap.id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthMap.id);

        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, OBSERVER_FBO_WIDTH, OBSERVER_FBO_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthMap.id, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    mDepthMap.width = OBSERVER_FBO_WIDTH;
    mDepthMap.height = OBSERVER_FBO_HEIGHT;
    mDepthMap.target = GL_TEXTURE_CUBE_MAP;
    mDepthMap.name = "depthMap";
    mDepthMap.unit = 1;

    mTerrain = mTerrainRenderer->GetTerrain();
}

void LineOfSightAnalyzer::LineOfSightRenderer::Render(float ifps)
{
    // Update observers' position
    for (int i = 0; i < 6; i++)
    {
        if (!mLockObserverPosition)
        {
            mObservers[i]->SetPosition(mTerrainRenderer->GetMouseWorldPosition() + QVector3D(0, mObserverHeight, 0));
        }

        mObservers[i]->SetZNear(mMinLosDistance);
        mObservers[i]->SetZFar(mMaxLosDistance);
    }

    // Line of sight render
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    glViewport(0, 0, OBSERVER_FBO_WIDTH, OBSERVER_FBO_HEIGHT);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mObserverShader->Bind();

    for (int i = 0; i < 6; i++)
    {
        mObserverShader->SetUniformValue(QString("VPs[%1]").arg(i), mObservers[i]->GetViewProjectionMatrix());
    }

    mObserverShader->SetUniformValue("minElevation", mTerrain->GetMinElevation());
    mObserverShader->SetUniformValue("maxElevation", mTerrain->GetMaxElevation());
    mObserverShader->SetUniformValue("observerPosition", mObservers[0]->GetPosition());
    mObserverShader->SetUniformValue("farPlane", mObservers[0]->GetZFar());
    mObserverShader->ActivateTexture(mTerrain->GetTexture());
    mTerrain->Render();
    mObserverShader->Release();
}

void LineOfSightAnalyzer::LineOfSightRenderer::DrawGui()
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Observer Settings");
    ImGui::SliderFloat("Observer Height", &mObserverHeight, 1.0f, 500.0f);
    ImGui::SliderFloat("Min LOS Distance", &mMinLosDistance, 0.1f, 100.f);
    ImGui::SliderFloat("Max LOS Distance", &mMaxLosDistance, mMinLosDistance, 1000.0f);
    ImGui::Checkbox("Lock Observer Position (L)", &mLockObserverPosition);
}

void LineOfSightAnalyzer::LineOfSightRenderer::SetTerrainRenderer(TerrainRenderer* terrainRenderer)
{
    mTerrainRenderer = terrainRenderer;
}

QVector3D LineOfSightAnalyzer::LineOfSightRenderer::GetObserverPosition() const
{
    return mObservers[0]->GetPosition();
}
