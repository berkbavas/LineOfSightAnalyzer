#include "Window.h"

#include "Util/Logger.h"

#include <QDateTime>
#include <QDebug>
#include <QKeyEvent>

LineOfSightAnalyzer::Window::Window(QWindow* parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)

{
    connect(this, &QOpenGLWindow::frameSwapped, [=]()
            { update(); });
}

void LineOfSightAnalyzer::Window::initializeGL()
{
    initializeOpenGLFunctions();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    emit Initialize();
}

void LineOfSightAnalyzer::Window::resizeGL(int width, int height)
{
    emit Resize(width, height);
}

void LineOfSightAnalyzer::Window::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    const float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    emit Render(ifps);
}

void LineOfSightAnalyzer::Window::keyPressEvent(QKeyEvent* event)
{
    emit KeyPressed(event);
}

void LineOfSightAnalyzer::Window::keyReleaseEvent(QKeyEvent* event)
{
    emit KeyReleased(event);
}

void LineOfSightAnalyzer::Window::mousePressEvent(QMouseEvent* event)
{
    emit MousePressed(event);
}

void LineOfSightAnalyzer::Window::mouseReleaseEvent(QMouseEvent* event)
{
    emit MouseReleased(event);
}

void LineOfSightAnalyzer::Window::mouseMoveEvent(QMouseEvent* event)
{
    emit MouseMoved(event);
}

void LineOfSightAnalyzer::Window::wheelEvent(QWheelEvent* event)
{
    emit WheelMoved(event);
}
