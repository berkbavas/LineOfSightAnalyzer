#include "LineOfSightRenderer.h"

#include "Camera/FreeCamera.h"
#include "Core/Constants.h"
#include "Manager/TerrainRenderer.h"

#include <imgui.h>

#include <QtImgui.h>

LineOfSightAnalyzer::LineOfSightRenderer::LineOfSightRenderer()
{
    initializeOpenGLFunctions();

    mObserverShader = std::make_unique<Shader>("Observer Shader");
    mObserverShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
    mObserverShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tcs");
    mObserverShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tes");
    mObserverShader->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/Observer.geom");
    mObserverShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Observer.frag");
    mObserverShader->Initialize();

    CreateObservers();

    mObserverFramebuffer = std::make_unique<CubicFramebuffer>(mFramebufferWidth, mFramebufferHeight);
}

void LineOfSightAnalyzer::LineOfSightRenderer::Render(float Ifps)
{
    UpdateObservers();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    mObserverFramebuffer->Clear();
    mObserverFramebuffer->Bind();

    glViewport(0, 0, mObserverFramebuffer->GetWidth(), mObserverFramebuffer->GetHeight());

    mObserverShader->Bind();

    for (int i = 0; i < mObservers.size(); i++)
    {
        mObserverShader->SetUniform(QString("uVPs[%1]").arg(i), mObservers.at(i)->GetViewProjectionMatrix());
    }

    mObserverShader->SetUniform("uMinimumElevation", mTerrain->GetMinElevation());
    mObserverShader->SetUniform("uMaximumElevation", mTerrain->GetMaxElevation());
    mObserverShader->SetUniform("uObserverPosition", GetObserverPosition());
    mObserverShader->SetUniform("uFarPlane", mMaxLosDistance);
    mObserverShader->SetSampler("uHeightMap", 0, mTerrain->GetTexture().Id, GL_TEXTURE_2D);
    mTerrain->Render();
    mObserverShader->Unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LineOfSightAnalyzer::LineOfSightRenderer::DrawGui()
{
    if (ImGui::CollapsingHeader("Line Of Sight Settings"))
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Observer Settings");
        ImGui::SliderFloat("Observer Height", &mObserverHeight, 0.0f, 250.0f);
        ImGui::SliderFloat("Min LOS Distance", &mMinLosDistance, 0.0f, 100.f);
        ImGui::SliderFloat("Max LOS Distance", &mMaxLosDistance, mMinLosDistance, 1000.0f);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Observer Mode");

        const char* ObserverModes[] = { "Full Sphere (360°)", "Hemisphere (Above Horizon)", "Directional Cone" };
        int CurrentMode = static_cast<int>(mObserverMode);
        if (ImGui::Combo("Mode", &CurrentMode, ObserverModes, IM_ARRAYSIZE(ObserverModes)))
        {
            mObserverMode = static_cast<ObserverMode>(CurrentMode);
        }

        switch (mObserverMode)
        {
        case ObserverMode::Hemisphere:
            ImGui::SliderFloat("Min Vertical Angle", &mVerticalAngleMin, -90.0f, 90.0f);
            ImGui::SliderFloat("Max Vertical Angle", &mVerticalAngleMax, mVerticalAngleMin, 90.0f);
            break;

        case ObserverMode::DirectionalCone:
            ImGui::SliderFloat("Cone Half-Angle", &mConeAngle, 5.0f, 90.0f);
            ImGui::SliderFloat("Direction (Yaw)", &mConeDirection, 0.0f, 360.0f);
            ImGui::SliderFloat("Pitch", &mConePitch, -90.0f, 90.0f);
            break;

        default:
            break;
        }
    }
}

void LineOfSightAnalyzer::LineOfSightRenderer::SetTerrain(Terrain* pTerrain)
{
    mTerrain = pTerrain;
}

QVector3D LineOfSightAnalyzer::LineOfSightRenderer::GetObserverPosition() const
{
    return mObservers.at(0)->GetPosition();
}

GLuint LineOfSightAnalyzer::LineOfSightRenderer::GetDepthMap() const
{
    return mObserverFramebuffer->GetDepthMap();
}

void LineOfSightAnalyzer::LineOfSightRenderer::SetObserverPositionOnTerrain(const QVector3D& Position)
{
    mObserverPositionOnTerrain = Position;
}

void LineOfSightAnalyzer::LineOfSightRenderer::CreateObservers()
{
    for (int i = 0; i < 6; i++)
    {
        mObservers.push_back(std::make_unique<DummyCamera>());
        mObservers.at(i)->Resize(mFramebufferWidth, mFramebufferHeight);
        mObservers.at(i)->SetVerticalFov(90.0f);
        mObservers.at(i)->SetZNear(mMinLosDistance);
        mObservers.at(i)->SetZFar(mMaxLosDistance);
    }

    // Why do we need to apply a roll fix?
    const auto RollFix = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), 180);

    mObservers.at(0)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), -90) * RollFix);
    mObservers.at(1)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 90) * RollFix);
    mObservers.at(2)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90));
    mObservers.at(3)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
    mObservers.at(4)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180) * RollFix);
    mObservers.at(5)->SetRotation(RollFix);
}

void LineOfSightAnalyzer::LineOfSightRenderer::UpdateObservers()
{
    for (const auto& pObserver : mObservers)
    {
        pObserver->SetPosition(mObserverPositionOnTerrain + QVector3D(0, mObserverHeight, 0));
        pObserver->SetZNear(mMinLosDistance);
        pObserver->SetZFar(mMaxLosDistance);
    }
}
