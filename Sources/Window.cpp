#include "Window.h"
#include "Controller.h"

#include <QDateTime>
#include <QKeyEvent>

#include <QDebug>

LineOfSightAnalyzer::Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)

{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(0);
    format.setSwapInterval(1);
    setFormat(format);

    connect(this, &QOpenGLWindow::frameSwapped, this, [=]() { update(); });
}

void LineOfSightAnalyzer::Window::initializeGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    QtImGui::initialize(this);

    mController = new Controller;
    mController->SetWindow(this);
    mController->Init();
}

void LineOfSightAnalyzer::Window::resizeGL(int w, int h)
{
    mController->Resize(w, h);
}

void LineOfSightAnalyzer::Window::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    mController->Render(ifps);
}

void LineOfSightAnalyzer::Window::keyPressEvent(QKeyEvent *event)
{
    mController->KeyPressed(event);
}

void LineOfSightAnalyzer::Window::keyReleaseEvent(QKeyEvent *event)
{
    mController->KeyReleased(event);
}

void LineOfSightAnalyzer::Window::mousePressEvent(QMouseEvent *event)
{
    mController->MousePressed(event);
}

void LineOfSightAnalyzer::Window::mouseReleaseEvent(QMouseEvent *event)
{
    mController->MouseReleased(event);
}

void LineOfSightAnalyzer::Window::mouseMoveEvent(QMouseEvent *event)
{
    mController->MouseMoved(event);
}

void LineOfSightAnalyzer::Window::wheelEvent(QWheelEvent *event)
{
    mController->WheelMoved(event);
}
