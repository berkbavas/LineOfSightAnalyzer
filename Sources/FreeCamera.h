#pragma once

#include "Mouse.h"
#include "Camera.h"

#include <QObject>

namespace LineOfSightAnalyzer
{
    class FreeCamera : public Camera
    {
    public:
        explicit FreeCamera(QObject* parent = nullptr);
        virtual ~FreeCamera();

        void KeyPressed(QKeyEvent* event) override;
        void KeyReleased(QKeyEvent* event) override;
        void MousePressed(QMouseEvent* event) override;
        void MouseReleased(QMouseEvent* event) override;
        void MouseMoved(QMouseEvent* event) override;
        void WheelMoved(QWheelEvent* event) override;
        void Update(float ifps) override;

        QMatrix4x4 GetProjectionMatrix() override;

        QVector3D GetEulerDegrees() const;

        DEFINE_MEMBER(float, VerticalFov);

    private:
        QMap<Qt::Key, bool> mPressedKeys;

        Mouse mMouse;

        bool mUpdateRotation;
        bool mUpdatePosition;

        static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
    };
}
