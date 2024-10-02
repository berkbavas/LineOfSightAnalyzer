#pragma once

#include "Core/Constants.h"
#include "Core/EventListener.h"
#include "Node/Node.h"

namespace LineOfSightAnalyzer
{
    class Camera : public Node, public EventListener
    {
      protected:
        Camera() = default;
        ~Camera() = default;

      public:
        virtual void Update(float ifps) = 0;
        virtual void Reset() {};
        virtual void Resize(int w, int h);

        virtual const QMatrix4x4& GetProjectionMatrix() = 0;

        QMatrix4x4 GetViewProjectionMatrix();
        QMatrix4x4 GetRotationMatrix();
        QMatrix4x4 GetViewMatrix();
        QVector3D GetViewDirection();

        float GetAspectRatio() const;

        DEFINE_MEMBER(int, Width, INITIAL_WIDTH);
        DEFINE_MEMBER(int, Height, INITIAL_HEIGHT);
        DEFINE_MEMBER(float, ZNear, 1.0f);
        DEFINE_MEMBER(float, ZFar, 100000.0f);
        DEFINE_MEMBER(float, DevicePixelRatio, 1.0f);
    };

    using CameraPtr = QSharedPointer<Camera>;
}
