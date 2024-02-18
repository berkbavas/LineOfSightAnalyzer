#pragma once

#include "Camera.h"
#include "Mouse.h"

#include <QObject>

namespace LineOfSightAnalyzer
{
    class OrthographicCamera : public Camera
    {
    public:
        explicit OrthographicCamera(QObject* parent = nullptr);

        virtual void KeyPressed(QKeyEvent*) override;
        virtual void KeyReleased(QKeyEvent*) override;
        virtual void MousePressed(QMouseEvent* event) override;
        virtual void MouseReleased(QMouseEvent* event) override;
        virtual void MouseMoved(QMouseEvent* event) override;
        virtual void WheelMoved(QWheelEvent* event) override;
        virtual void Update(float) override;

        virtual QMatrix4x4 GetProjectionMatrix() override;

    private:
        DEFINE_MEMBER(float, Zoom);
        DEFINE_MEMBER(float, Left);
        DEFINE_MEMBER(float, Top);

        Mouse mMouse;
        bool mUpdatePosition;

        float mTimeElapsed;
    };
}
