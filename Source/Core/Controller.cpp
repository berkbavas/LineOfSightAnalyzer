#include "Controller.h"

#include "Util/Logger.h"

#include <imgui.h>

#include <QImageReader>
#include <QOpenGLFramebufferObject>

LineOfSightAnalyzer::Controller::Controller(QObject* pParent)
    : QObject(pParent)
{
    mWindow = std::make_unique<OpenGLWidget>();

    connect(mWindow.get(), &OpenGLWidget::Initialized, this, &Controller::Initialize);
    connect(mWindow.get(), &OpenGLWidget::Resized, this, &Controller::Resize);
    connect(mWindow.get(), &OpenGLWidget::Render, this, &Controller::Render);
    connect(mWindow.get(), &OpenGLWidget::KeyPressed, this, &Controller::OnKeyPressed);
    connect(mWindow.get(), &OpenGLWidget::KeyReleased, this, &Controller::OnKeyReleased);
    connect(mWindow.get(), &OpenGLWidget::MousePressed, this, &Controller::OnMousePressed);
    connect(mWindow.get(), &OpenGLWidget::MouseReleased, this, &Controller::OnMouseReleased);
    connect(mWindow.get(), &OpenGLWidget::MouseMoved, this, &Controller::OnMouseMoved);
    connect(mWindow.get(), &OpenGLWidget::WheelMoved, this, &Controller::OnWheelMoved);
    connect(mWindow.get(), &OpenGLWidget::LeaveEvent, this, &Controller::OnLeaveEvent);
    connect(mWindow.get(), &OpenGLWidget::FocusOutEvent, this, &Controller::OnFocusOutEvent);
}

void LineOfSightAnalyzer::Controller::Run()
{
    QImageReader::setAllocationLimit(0);
    mWindow->showMaximized();
}

void LineOfSightAnalyzer::Controller::Initialize()
{
    initializeOpenGLFunctions();
    glDisable(GL_MULTISAMPLE);

    mCameraManager = std::make_unique<CameraManager>();
    mLineOfSightRenderer = std::make_unique<LineOfSightRenderer>();
    mTerrainRenderer = std::make_unique<TerrainRenderer>();

    mTerrainRenderer->SetLineOfSightRenderer(mLineOfSightRenderer.get());
    mLineOfSightRenderer->SetTerrainRenderer(mTerrainRenderer.get());
    mLineOfSightRenderer->SetTerrain(mTerrainRenderer->GetTerrain());

    mRenderRef = QtImGui::initialize(mWindow.get(), true);

    mEventReceivers.append(mCameraManager.get());
    mEventReceivers.append(mTerrainRenderer.get());
    mEventReceivers.append(mLineOfSightRenderer.get());
}

void LineOfSightAnalyzer::Controller::Resize(int Width, int Height)
{
    mCameraManager->Resize(Width, Height);
    mTerrainRenderer->Resize(Width, Height);
}

void LineOfSightAnalyzer::Controller::Render(float Ifps)
{
    const auto pActiveCamera = mCameraManager->GetActiveCamera();
    const auto DevicePixelRatio = mWindow->devicePixelRatio();
    const auto Width = mWindow->width();
    const auto Height = mWindow->height();

    mCameraManager->Update(Ifps);
    mLineOfSightRenderer->Render(Ifps);
    mTerrainRenderer->Render(pActiveCamera, DevicePixelRatio, Ifps);

    // Render the final output to the default framebuffer
    QOpenGLFramebufferObject::bindDefault();
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, Width * DevicePixelRatio, Height * DevicePixelRatio);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QOpenGLFramebufferObject::blitFramebuffer(nullptr, // Blit to the default framebuffer
                                              QRect(0, 0, Width * DevicePixelRatio, Height * DevicePixelRatio),
                                              mTerrainRenderer->GetFramebuffer()->GetFramebufferObject(),
                                              mTerrainRenderer->GetFramebuffer()->GetViewport(),
                                              GL_COLOR_BUFFER_BIT,
                                              GL_NEAREST,
                                              0,
                                              0);

    QtImGui::newFrame(mRenderRef);

    ImGui::Begin("Settings");
    mTerrainRenderer->DrawGui();
    mLineOfSightRenderer->DrawGui();
    mCameraManager->DrawGui();
    DrawStats();
    ImGui::End();

    ImGui::Render();
    QtImGui::render(mRenderRef);
}

void LineOfSightAnalyzer::Controller::OnKeyPressed(QKeyEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return;
    }

    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->OnKeyPressed(pEvent))
        {
            return;
        }
    }
}

void LineOfSightAnalyzer::Controller::OnKeyReleased(QKeyEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return;
    }

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->OnKeyReleased(pEvent);
    }
}

void LineOfSightAnalyzer::Controller::OnMousePressed(QMouseEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->OnMousePressed(pEvent))
        {
            return;
        }
    }
}

void LineOfSightAnalyzer::Controller::OnMouseReleased(QMouseEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->OnMouseReleased(pEvent);
    }
}

void LineOfSightAnalyzer::Controller::OnMouseMoved(QMouseEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->OnMouseMoved(pEvent))
        {
            return;
        }
    }
}

void LineOfSightAnalyzer::Controller::OnWheelMoved(QWheelEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    for (const auto pReceiver : mEventReceivers)
    {
        if (pReceiver->OnWheelMoved(pEvent))
        {
            return;
        }
    }
}

void LineOfSightAnalyzer::Controller::OnLeaveEvent(QEvent* pEvent)
{
    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->OnLeaveEvent(pEvent);
    }
}

void LineOfSightAnalyzer::Controller::OnFocusOutEvent(QFocusEvent* pEvent)
{
    for (const auto pReceiver : mEventReceivers)
    {
        pReceiver->OnFocusOutEvent(pEvent);
    }
}

void LineOfSightAnalyzer::Controller::DrawStats()
{
    if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const auto& MouseWorldPosition = mTerrainRenderer->GetMouseWorldPosition();
        ImGui::Text("Mouse World Position: (%.1f, %.1f, %.1f)", MouseWorldPosition[0], MouseWorldPosition[1], MouseWorldPosition[2]);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
}
