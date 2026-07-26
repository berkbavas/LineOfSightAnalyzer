#pragma once

#include "Camera/Camera.h"

namespace LineOfSightAnalyzer
{
    class PerspectiveCamera : public Camera
    {
      public:
        PerspectiveCamera() = default;

        QMatrix4x4 GetProjectionMatrix() const override;
        float GetHorizontalFov() const;
        float GetAspectRatio() const;
        float GetVerticalFov() const;
        void SetVerticalFov(float VerticalFov);

      private:
        float mVerticalFov{ 45.0f }; // Vertical field of view in degrees
    };

    using PerspectiveCameraPtr = std::unique_ptr<PerspectiveCamera>;
}