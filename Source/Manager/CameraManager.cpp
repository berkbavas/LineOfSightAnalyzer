#include "CameraManager.h"

#include <imgui.h>

#include <QtImgui.h>

LineOfSightAnalyzer::CameraManager::CameraManager()
{
    mFreeCamera = std::make_unique<FreeCamera>();
    mFreeCamera->SetPosition(QVector3D(0, 1000, 0));
    mFreeCamera->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
    mFreeCamera->SetVerticalFov(50.0f);

    mOrthographicCamera = std::make_unique<OrthographicCamera>();
    mOrthographicCamera->SetPosition(QVector3D(0, 10000, 0));
    mOrthographicCamera->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));

    SetActiveCamera(mOrthographicCamera.get());
}

bool LineOfSightAnalyzer::CameraManager::OnKeyPressed(QKeyEvent* pEvent)
{
    bool Consumed = false;

    if (pEvent->key() == Qt::Key_1)
    {
        SetActiveCamera(mFreeCamera.get());
        Consumed = true;
    }
    else if (pEvent->key() == Qt::Key_2)
    {
        SetActiveCamera(mOrthographicCamera.get());
        Consumed = true;
    }
    else
    {
        Consumed = mActiveCamera->OnKeyPressed(pEvent);
    }

    return Consumed;
}

bool LineOfSightAnalyzer::CameraManager::OnKeyReleased(QKeyEvent* pEvent)
{
    return mActiveCamera->OnKeyReleased(pEvent);
}

bool LineOfSightAnalyzer::CameraManager::OnMousePressed(QMouseEvent* pEvent)
{
    return mActiveCamera->OnMousePressed(pEvent);
}

bool LineOfSightAnalyzer::CameraManager::OnMouseReleased(QMouseEvent* pEvent)
{
    return mActiveCamera->OnMouseReleased(pEvent);
}

bool LineOfSightAnalyzer::CameraManager::OnMouseMoved(QMouseEvent* pEvent)
{
    return mActiveCamera->OnMouseMoved(pEvent);
}

bool LineOfSightAnalyzer::CameraManager::OnWheelMoved(QWheelEvent* pEvent)
{
    return mActiveCamera->OnWheelMoved(pEvent);
}

bool LineOfSightAnalyzer::CameraManager::OnLeaveEvent(QEvent* pEvent)
{
    return mActiveCamera->OnLeaveEvent(pEvent);
}

bool LineOfSightAnalyzer::CameraManager::OnFocusOutEvent(QFocusEvent* pEvent)
{
    return mActiveCamera->OnFocusOutEvent(pEvent);
}

void LineOfSightAnalyzer::CameraManager::Update(float Ifps)
{
    mActiveCamera->Update(Ifps);
}

void LineOfSightAnalyzer::CameraManager::DrawGui()
{
    if (ImGui::CollapsingHeader("Camera Hints", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("1   : Free Camera");
        ImGui::Text("2   : Orthographic Camera");
        ImGui::Text(">>> : Shift");
        ImGui::Text("|   : Ctrl");
        ImGui::Text("L   : Lock Observer");
    }
}

void LineOfSightAnalyzer::CameraManager::Resize(int Width, int Height)
{
    mActiveCamera->Resize(Width, Height);
}

void LineOfSightAnalyzer::CameraManager::SetActiveCamera(Camera* pCamera)
{
    if (mActiveCamera)
    {
        pCamera->Resize(mActiveCamera->GetWidth(), mActiveCamera->GetHeight());
        mActiveCamera->Reset();
    }

    mActiveCamera = pCamera;
    mActiveCamera->Reset();
}

LineOfSightAnalyzer::Camera* LineOfSightAnalyzer::CameraManager::GetActiveCamera() const
{
    return mActiveCamera;
}
