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

    mDepthMap.Id = CreateLineOfSightFramebuffer(OBSERVER_FBO_WIDTH, OBSERVER_FBO_HEIGHT);
    mDepthMap.Width = OBSERVER_FBO_WIDTH;
    mDepthMap.Height = OBSERVER_FBO_HEIGHT;
    mDepthMap.Target = GL_TEXTURE_CUBE_MAP;
    mDepthMap.Name = "uDepthMap";
    mDepthMap.Unit = 1;
}

void LineOfSightAnalyzer::LineOfSightRenderer::Render(float Ifps)
{
    // Update observers' position
    for (int i = 0; i < mObservers.size(); i++)
    {
        if (!mLockObserverPosition)
        {
            mObservers.at(i)->SetPosition(mTerrainRenderer->GetMouseWorldPosition() + QVector3D(0, mObserverHeight, 0));
        }

        mObservers.at(i)->SetZNear(mMinLosDistance);
        mObservers.at(i)->SetZFar(mMaxLosDistance);
    }

    // Line of sight render
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, OBSERVER_FBO_WIDTH, OBSERVER_FBO_HEIGHT);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    mObserverShader->Bind();

    for (int i = 0; i < mObservers.size(); i++)
    {
        mObserverShader->SetUniform(QString("uVPs[%1]").arg(i), mObservers.at(i)->GetViewProjectionMatrix());
    }

    mObserverShader->SetUniform("uMinimumElevation", mTerrain->GetMinElevation());
    mObserverShader->SetUniform("uMaximumElevation", mTerrain->GetMaxElevation());
    mObserverShader->SetUniform("uObserverPosition", mObservers.at(0)->GetPosition());
    mObserverShader->SetUniform("uFarPlane", mObservers.at(0)->GetZFar());
    mObserverShader->SetSampler(mTerrain->GetTexture().Name, mTerrain->GetTexture().Unit, mTerrain->GetTexture().Id, mTerrain->GetTexture().Target);
    mTerrain->Render();
    mObserverShader->Unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LineOfSightAnalyzer::LineOfSightRenderer::DrawGui()
{
    if (ImGui::CollapsingHeader("Line Of Sight Settings"))
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Observer Settings");
        ImGui::SliderFloat("Observer Height", &mObserverHeight, 1.0f, 500.0f);
        ImGui::SliderFloat("Min LOS Distance", &mMinLosDistance, 0.1f, 100.f);
        ImGui::SliderFloat("Max LOS Distance", &mMaxLosDistance, mMinLosDistance, 1000.0f);
        ImGui::Checkbox("Lock Observer Position (L)", &mLockObserverPosition);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Observer Mode");

        const char* ObserverModes[] = { "Full Sphere (360°)", "Hemisphere (Above Horizon)", "Directional Cone" };
        int currentMode = static_cast<int>(mObserverMode);
        if (ImGui::Combo("Mode", &currentMode, ObserverModes, IM_ARRAYSIZE(ObserverModes)))
        {
            mObserverMode = static_cast<ObserverMode>(currentMode);
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

void LineOfSightAnalyzer::LineOfSightRenderer::SetTerrainRenderer(TerrainRenderer* pTerrainRenderer)
{
    mTerrainRenderer = pTerrainRenderer;
}

void LineOfSightAnalyzer::LineOfSightRenderer::SetTerrain(Terrain* pTerrain)
{
    mTerrain = pTerrain;
}

bool LineOfSightAnalyzer::LineOfSightRenderer::OnKeyPressed(QKeyEvent* pEvent)
{
    if (pEvent->key() == Qt::Key_L)
    {
        mLockObserverPosition = !mLockObserverPosition;
        return true;
    }

    return false;
}

QVector3D LineOfSightAnalyzer::LineOfSightRenderer::GetObserverPosition() const
{
    return mObservers.at(0)->GetPosition();
}

void LineOfSightAnalyzer::LineOfSightRenderer::CreateObservers()
{
    for (int i = 0; i < NUMBER_OF_OBSERVERS; i++)
    {
        mObservers.push_back(std::make_unique<FreeCamera>());
        mObservers.at(i)->SetPosition(QVector3D(0, 100, 0));
        mObservers.at(i)->Resize(OBSERVER_FBO_WIDTH, OBSERVER_FBO_HEIGHT);
        mObservers.at(i)->SetVerticalFov(90.0f);
        mObservers.at(i)->SetZNear(mMinLosDistance);
        mObservers.at(i)->SetZFar(mMaxLosDistance);
    }
    const auto RollFix = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), 180);

    mObservers.at(0)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), -90) * RollFix);
    mObservers.at(1)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 90) * RollFix);
    mObservers.at(2)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90));
    mObservers.at(3)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
    mObservers.at(4)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180) * RollFix);
    mObservers.at(5)->SetRotation(RollFix);
}

GLuint LineOfSightAnalyzer::LineOfSightRenderer::CreateLineOfSightFramebuffer(int Width, int Height)
{
    GLuint TextureId;

    glGenFramebuffers(1, &mFramebuffer);
    glGenTextures(1, &TextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, //
                     0,
                     GL_DEPTH_COMPONENT,
                     Width,
                     Height,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    return TextureId;
}
