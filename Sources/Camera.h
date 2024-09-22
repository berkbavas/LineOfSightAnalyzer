#pragma once

#include "Common.h"
#include "Node.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QTimer>

namespace LineOfSightAnalyzer
{
    class Camera : public Node
    {
        Q_OBJECT
    public:
        explicit Camera(QObject* parent = nullptr);

        virtual void KeyPressed(QKeyEvent*) = 0;
        virtual void KeyReleased(QKeyEvent*) = 0;
        virtual void MousePressed(QMouseEvent* event) = 0;
        virtual void MouseReleased(QMouseEvent* event) = 0;
        virtual void MouseMoved(QMouseEvent* event) = 0;
        virtual void WheelMoved(QWheelEvent* event) = 0;
        virtual void Resize(int width, int height);
        virtual void Update(float) = 0;

        virtual QMatrix4x4 GetProjectionMatrix() = 0;
        virtual QMatrix4x4 GetViewMatrix();
        virtual QMatrix4x4 GetViewProjectionMatrix();
        virtual QMatrix4x4 GetRotationMatrix();
        virtual QVector3D GetViewDirection();

        bool GetActive() const;
        void SetActive(bool newActive);

    signals:
        void ActiveChanged();

    protected:
        bool mActive;

        DEFINE_MEMBER(int, Width);
        DEFINE_MEMBER(int, Height);
        DEFINE_MEMBER(float, DevicePixelRatio);
        DEFINE_MEMBER(float, ZNear);
        DEFINE_MEMBER(float, ZFar);
    };
}
