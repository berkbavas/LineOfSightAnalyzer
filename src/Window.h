#pragma once

#include <QOpenGLWindow>

#include <imgui.h>
#include <QtImGui.h>

namespace LineOfSightAnalyzer
{
    class Controller;

    class Window : public QOpenGLWindow
    {
    public:
        Window(QWindow* parent = nullptr);

    private:
        void initializeGL() override;
        void resizeGL(int w, int h) override;
        void paintGL() override;
        void keyPressEvent(QKeyEvent*) override;
        void keyReleaseEvent(QKeyEvent*) override;
        void mousePressEvent(QMouseEvent*) override;
        void mouseReleaseEvent(QMouseEvent*) override;
        void mouseMoveEvent(QMouseEvent*) override;
        void wheelEvent(QWheelEvent*) override;

    private:
        long long mPreviousTime;
        long long mCurrentTime;
        Controller* mController;
    };
}
