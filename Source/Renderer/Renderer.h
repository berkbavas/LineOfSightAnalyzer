#pragma once

#include "Node/Camera/Camera.h"

#include <QMouseEvent>
#include <QOpenGLExtraFunctions>

namespace LineOfSightAnalyzer
{
    class CameraManager;
    class TerrainRenderer;
    class LineOfSightRenderer;

    class Renderer : protected QOpenGLExtraFunctions
    {
        DISABLE_COPY(Renderer);

      public:
        Renderer() = default;

        void Initialize();

        void Render(float ifps);
        void DrawGui();
        void Resize(int w, int h);

        void onMouseMoved(QMouseEvent* event);

      private:
        TerrainRenderer* mTerrainRenderer;
        LineOfSightRenderer* mLineOfSightRenderer;

        Camera* mActiveCamera;

        DEFINE_MEMBER_PTR(CameraManager, CameraManager);

        DEFINE_MEMBER(float, DevicePixelRatio, 1.0f);
    };
}