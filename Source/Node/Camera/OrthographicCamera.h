#pragma once

#include "Node/Camera/Camera.h"
#include "Structs/Mouse.h"

#include <QObject>

namespace LineOfSightAnalyzer
{
    class OrthographicCamera : public Camera
    {
      public:
        OrthographicCamera() = default;

        void OnMousePressed(QMouseEvent* event) override;
        void OnMouseReleased(QMouseEvent* event) override;
        void OnMouseMoved(QMouseEvent* event) override;
        void OnWheelMoved(QWheelEvent* event) override;
        void Update(float) override;

        const QMatrix4x4& GetProjectionMatrix() override;

      private:
        DEFINE_MEMBER(float, Zoom, 2.0f);
        DEFINE_MEMBER(float, Left, 0.0f);
        DEFINE_MEMBER(float, Top, 0.0f);
        DEFINE_MEMBER(Qt::MouseButton, ActionReceiveButton, Qt::LeftButton)

        Mouse mMouse;
        bool mUpdatePosition{ false };

        float mTimeElapsed{ 0 };

        QMatrix4x4 mProjectionMatrix;
    };
}
