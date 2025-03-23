
#pragma once

#include "Core/EventListener.h"
#include "Node/Camera/FreeCamera.h"
#include "Node/Camera/OrthographicCamera.h"

namespace LineOfSightAnalyzer
{

    class CameraManager : public EventListener
    {
      public:
        CameraManager();

        void OnKeyPressed(QKeyEvent*) override;
        void OnKeyReleased(QKeyEvent*) override;
        void OnMousePressed(QMouseEvent*) override;
        void OnMouseReleased(QMouseEvent*) override;
        void OnMouseMoved(QMouseEvent*) override;
        void OnWheelMoved(QWheelEvent*) override;

        void Update(float ifps);
        void DrawGui();
        void Resize(int w, int h);
        void SetDevicePixelRatio(float value);

        void SetActiveCamera(Camera* camera);
        Camera* GetActiveCamera() const;
        FreeCamera* GetFreeCamera() const;

      private:
        Camera* mActiveCamera{ nullptr };
        FreeCamera* mFreeCamera{ nullptr };
        OrthographicCamera* mOrthographicCamera{ nullptr };
    };
}