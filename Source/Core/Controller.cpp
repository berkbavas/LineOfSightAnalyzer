#include "Controller.h"

#include "Core/CameraManager.h"
#include "Core/Window.h"
#include "Renderer/Renderer.h"
#include "Util/Logger.h"

#include <QtImGui.h>
#include <imgui.h>

LineOfSightAnalyzer::Controller::Controller(QObject* parent)
    : QObject(parent)
{
    mWindow = new Window;
    mRenderer = new Renderer;
    mCameraManager = new CameraManager;

    connect(mWindow, &LineOfSightAnalyzer::Window::Initialize, this, &LineOfSightAnalyzer::Controller::Initialize);
    connect(mWindow, &LineOfSightAnalyzer::Window::Render, this, &LineOfSightAnalyzer::Controller::Render);
    connect(mWindow, &LineOfSightAnalyzer::Window::Resize, this, &LineOfSightAnalyzer::Controller::Resize);
    connect(mWindow, &LineOfSightAnalyzer::Window::MousePressed, this, &LineOfSightAnalyzer::Controller::OnMousePressed);
    connect(mWindow, &LineOfSightAnalyzer::Window::MouseReleased, this, &LineOfSightAnalyzer::Controller::OnMouseReleased);
    connect(mWindow, &LineOfSightAnalyzer::Window::MouseMoved, this, &LineOfSightAnalyzer::Controller::OnMouseMoved);
    connect(mWindow, &LineOfSightAnalyzer::Window::WheelMoved, this, &LineOfSightAnalyzer::Controller::OnWheelMoved);
    connect(mWindow, &LineOfSightAnalyzer::Window::KeyPressed, this, &LineOfSightAnalyzer::Controller::OnKeyPressed);
    connect(mWindow, &LineOfSightAnalyzer::Window::KeyReleased, this, &LineOfSightAnalyzer::Controller::OnKeyReleased);
}

LineOfSightAnalyzer::Controller::~Controller()
{
    LOG_DEBUG("Controller::~Controller: Application closing...");
}

void LineOfSightAnalyzer::Controller::Run()
{
    LOG_DEBUG("Controller::Controller: Application starting...");

    mWindow->resize(mWidth, mHeight);
    mWindow->show();
}

void LineOfSightAnalyzer::Controller::Initialize()
{
    initializeOpenGLFunctions();

    mRenderer->SetCameraManager(mCameraManager);
    mRenderer->Initialize();

    QtImGui::initialize(mWindow);
}

void LineOfSightAnalyzer::Controller::Render(float ifps)
{
    mDevicePixelRatio = mWindow->devicePixelRatio();
    mWidth = mWindow->width() * mDevicePixelRatio;
    mHeight = mWindow->height() * mDevicePixelRatio;

    mCameraManager->Update(ifps);
    mCameraManager->SetDevicePixelRatio(mDevicePixelRatio);

    mRenderer->Render(ifps);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, mWidth, mHeight);

    QtImGui::newFrame();

    ImGui::Begin("Settings");
    mRenderer->DrawGui();
    mCameraManager->DrawGui();
    ImGui::End();

    ImGui::End();

    ImGui::Render();
    QtImGui::render();
}

void LineOfSightAnalyzer::Controller::OnKeyPressed(QKeyEvent* event)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return;
    }

    mCameraManager->OnKeyPressed(event);
}

void LineOfSightAnalyzer::Controller::OnKeyReleased(QKeyEvent* event)
{
    mCameraManager->OnKeyReleased(event);
}

void LineOfSightAnalyzer::Controller::Resize(int width, int height)
{
    mDevicePixelRatio = mWindow->devicePixelRatio();
    mWidth = width * mDevicePixelRatio;
    mHeight = height * mDevicePixelRatio;

    mWindow->makeCurrent();
    mCameraManager->Resize(mWidth, mHeight);
    mRenderer->Resize(mWidth, mHeight);
    mWindow->doneCurrent();
}

void LineOfSightAnalyzer::Controller::OnMousePressed(QMouseEvent* event)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    mCameraManager->OnMousePressed(event);
}

void LineOfSightAnalyzer::Controller::OnMouseReleased(QMouseEvent* event)
{
    mCameraManager->OnMouseReleased(event);
}

void LineOfSightAnalyzer::Controller::OnMouseMoved(QMouseEvent* event)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    mCameraManager->OnMouseMoved(event);
    mRenderer->onMouseMoved(event);
}

void LineOfSightAnalyzer::Controller::OnWheelMoved(QWheelEvent* event)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    mCameraManager->OnWheelMoved(event);
}
