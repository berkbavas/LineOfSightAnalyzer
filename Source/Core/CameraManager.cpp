#include "CameraManager.h"

#include <QtImgui.h>
#include <imgui.h>

LineOfSightAnalyzer::CameraManager::CameraManager()
{
    mFreeCamera = new FreeCamera;

    mFreeCamera->SetPosition(QVector3D(0, 1000, 0));
    mFreeCamera->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
    mFreeCamera->SetVerticalFov(50.0f);
    mFreeCamera->SetZNear(1.0f);
    mFreeCamera->SetZFar(100000.0f);

    mOrthographicCamera = new OrthographicCamera;
    mOrthographicCamera->SetPosition(QVector3D(0, 10000, 0));
    mOrthographicCamera->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
    mOrthographicCamera->SetZNear(1.0f);
    mOrthographicCamera->SetZFar(100000.0f);

    SetActiveCamera(mOrthographicCamera);
}

void LineOfSightAnalyzer::CameraManager::OnKeyPressed(QKeyEvent* event)
{
    if (event->key() == Qt::Key_1)
    {
        SetActiveCamera(mFreeCamera);
    }
    else if (event->key() == Qt::Key_2)
    {
        SetActiveCamera(mOrthographicCamera);
    }
    else
    {
        mActiveCamera->OnKeyPressed(event);
    }
}

void LineOfSightAnalyzer::CameraManager::OnKeyReleased(QKeyEvent* event)
{
    mActiveCamera->OnKeyReleased(event);
}

void LineOfSightAnalyzer::CameraManager::OnMousePressed(QMouseEvent* event)
{
    mActiveCamera->OnMousePressed(event);
}

void LineOfSightAnalyzer::CameraManager::OnMouseReleased(QMouseEvent* event)
{
    mActiveCamera->OnMouseReleased(event);
}

void LineOfSightAnalyzer::CameraManager::OnMouseMoved(QMouseEvent* event)
{
    mActiveCamera->OnMouseMoved(event);
}

void LineOfSightAnalyzer::CameraManager::OnWheelMoved(QWheelEvent* event)
{
    mActiveCamera->OnWheelMoved(event);
}

void LineOfSightAnalyzer::CameraManager::Update(float ifps)
{
    mActiveCamera->Update(ifps);
}

void LineOfSightAnalyzer::CameraManager::DrawGui()
{
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Hints");
    ImGui::Text("1   : Free Camera");
    ImGui::Text("2   : Orthographic Camera");
    ImGui::Text(">   : Shift");
    ImGui::Text(">>> : Ctrl");
}

void LineOfSightAnalyzer::CameraManager::Resize(int w, int h)
{
    mActiveCamera->Resize(w, h);
}

void LineOfSightAnalyzer::CameraManager::SetDevicePixelRatio(float value)
{
    mActiveCamera->SetDevicePixelRatio(value);
}

void LineOfSightAnalyzer::CameraManager::SetActiveCamera(Camera* camera)
{
    if (mActiveCamera)
    {
        camera->Resize(mActiveCamera->GetWidth(), mActiveCamera->GetHeight());
        mActiveCamera->Reset();
    }

    mActiveCamera = camera;
    mActiveCamera->Reset();
    mActiveCamera->Update(0);
}

LineOfSightAnalyzer::Camera* LineOfSightAnalyzer::CameraManager::GetActiveCamera() const
{
    return mActiveCamera;
}
