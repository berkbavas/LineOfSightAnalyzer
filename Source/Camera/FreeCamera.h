#pragma once

#include "Camera/PerspectiveCamera.h"
#include "Core/Mouse.h"

namespace LineOfSightAnalyzer
{
    class FreeCamera final : public PerspectiveCamera
    {
      public:
        FreeCamera() = default;

        void Update(float Ifps) override;
        void Reset() override;
        bool OnKeyPressed(QKeyEvent *pEvent) override;
        bool OnKeyReleased(QKeyEvent *pEvent) override;
        bool OnMousePressed(QMouseEvent *pEvent) override;
        bool OnMouseReleased(QMouseEvent *pEvent) override;
        bool OnMouseMoved(QMouseEvent *pEvent) override;

      private:
        bool IsKeyPressed(Qt::Key Key) const;
        float CalculateLinearSpeed(float Ifps) const;
        float CalculateAngularSpeed(float Ifps) const;
        void ApplyRotation(float YawDelta, float PitchDelta);

        // State
        float mYaw{ 0.0f };   // Rotation around the Y-axis
        float mPitch{ 0.0f }; // Rotation around the X-axis

        QMap<Qt::Key, bool> mPressedKeys; // Tracks the state of keys (pressed or not)
        Mouse mMouse;                     // Tracks the current state of the mouse (position, movement, buttons)

        static const QMap<Qt::Key, QVector3D> KEY_BINDINGS; // Maps keys to their corresponding movement directions in the camera's local space

        // Traits
        DEFINE_MEMBER(float, AngularSpeed, 15.0f);
        DEFINE_MEMBER(float, LinearSpeed, 5.0f);
    };

    using FreeCameraPtr = std::unique_ptr<FreeCamera>;
}