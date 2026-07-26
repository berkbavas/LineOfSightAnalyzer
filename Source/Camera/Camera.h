#pragma once

#include "Core/Enums.h"
#include "Core/EventReceiver.h"
#include "Node/Node.h"

namespace LineOfSightAnalyzer
{
    class Camera : public Node, public EventReceiver
    {
      public:
        Camera() = default;

        virtual void Update(float Ifps) = 0;
        virtual void Reset() = 0;
        virtual QMatrix4x4 GetProjectionMatrix() const = 0;

        virtual void Resize(int Width, int Height);
        virtual QVector2D ProjectWorldToScreenSpace(const QVector3D& Position) const;
        virtual QVector3D ProjectScreenToWorldSpace(const QPointF& Point, float Depth) const;
        virtual float GetDepthOfPoint(const QVector3D& Point) const;
        virtual QVector3D ComputeRayFromScreen(const QPointF& Point) const;
        QMatrix4x4 GetViewProjectionMatrix() const;
        QMatrix4x4 GetRotationMatrix() const;
        QMatrix4x4 GetViewMatrix() const;
        QVector3D GetViewDirection() const;
        QVector3D GetDirection(ViewDirection ViewDirection) const;
        QVector2D GetViewportSize() const;
        int GetWidth() const;
        int GetHeight() const;
        float GetZNear() const;
        float GetZFar() const;
        void SetZNear(float ZNear);
        void SetZFar(float ZFar);

      protected:
        int mWidth{ 1 };
        int mHeight{ 1 };
        float mZNear{ 0.1f };     // Near clipping plane distance
        float mZFar{ 10'000.0f }; // Far clipping plane distance
    };

    using CameraPtr = std::unique_ptr<Camera>;
}