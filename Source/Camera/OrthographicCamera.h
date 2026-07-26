#pragma once

#include "Camera/Camera.h"
#include "Core/Mouse.h"

#include <QObject>

namespace LineOfSightAnalyzer
{
    class OrthographicCamera : public Camera
    {
      public:
        OrthographicCamera() = default;

        void Update(float Ifps) override;
        void Reset() override;

        bool OnMousePressed(QMouseEvent* pEvent) override;
        bool OnMouseReleased(QMouseEvent* pEvent) override;
        bool OnMouseMoved(QMouseEvent* pEvent) override;
        bool OnWheelMoved(QWheelEvent* pEvent) override;
        bool OnLeaveEvent(QEvent* pEvent) override;
        bool OnFocusOutEvent(QFocusEvent* pEvent) override;

        QMatrix4x4 GetProjectionMatrix() const override;

      private:
        float mZoom{ 2.0f };
        float mLeft{ 0.0f };
        float mTop{ 0.0f };

        Mouse mMouse;
        bool mUpdatePosition{ false };
    };

    using OrthographicCameraPtr = std::unique_ptr<OrthographicCamera>;
}
