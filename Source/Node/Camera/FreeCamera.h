#pragma once

#include "Node/Camera/PerspectiveCamera.h"
#include "Structs/Mouse.h"

#include <QMap>

namespace LineOfSightAnalyzer
{
    class FreeCamera : public PerspectiveCamera
    {
      public:
        FreeCamera() = default;

        void Update(float ifps) override;
        void Reset() override;

        void OnKeyPressed(QKeyEvent*) override;
        void OnKeyReleased(QKeyEvent*) override;
        void OnMousePressed(QMouseEvent*) override;
        void OnMouseReleased(QMouseEvent*) override;
        void OnMouseMoved(QMouseEvent*) override;

      private:
        DEFINE_MEMBER(float, AngularSpeed, 25.0f);
        DEFINE_MEMBER(float, LinearSpeed, 5.0f);
        DEFINE_MEMBER(float, LinearSpeedMultiplier, 1.0f);
        DEFINE_MEMBER(float, AngularSpeedMultiplier, 1.0f);
        DEFINE_MEMBER(Qt::MouseButton, ActionReceiveButton, Qt::LeftButton)

        QMap<Qt::Key, bool> mPressedKeys;

        Mouse mMouse;

        bool mUpdateRotation{ false };
        bool mUpdatePosition{ false };

        static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
    };
}