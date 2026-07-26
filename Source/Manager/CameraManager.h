
#pragma once

#include "Camera/FreeCamera.h"
#include "Camera/OrthographicCamera.h"
#include "Core/EventReceiver.h"

namespace LineOfSightAnalyzer
{
    class CameraManager : public EventReceiver
    {
      public:
        CameraManager();

        void Update(float Ifps);
        void DrawGui();
        void Resize(int Width, int Height);

        bool OnKeyPressed(QKeyEvent* pEvent) override;
        bool OnKeyReleased(QKeyEvent* pEvent) override;
        bool OnMousePressed(QMouseEvent* pEvent) override;
        bool OnMouseReleased(QMouseEvent* pEvent) override;
        bool OnMouseMoved(QMouseEvent* pEvent) override;
        bool OnWheelMoved(QWheelEvent* pEvent) override;
        bool OnLeaveEvent(QEvent* pEvent) override;
        bool OnFocusOutEvent(QFocusEvent* pEvent) override;

        Camera* GetActiveCamera() const;

      private:
        void SetActiveCamera(Camera* pCamera);

        Camera* mActiveCamera{ nullptr };
        FreeCameraPtr mFreeCamera{ nullptr };
        OrthographicCameraPtr mOrthographicCamera{ nullptr };
    };

    using CameraManagerPtr = std::unique_ptr<CameraManager>;
}