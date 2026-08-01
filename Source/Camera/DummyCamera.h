#pragma once

#include "Camera/PerspectiveCamera.h"

namespace LineOfSightAnalyzer
{
    class DummyCamera : public PerspectiveCamera
    {
      public:
        DummyCamera() = default;

        void Update(float Ifps) override;
        void Reset() override;
    };

    using DummyCameraPtr = std::unique_ptr<DummyCamera>;
}
