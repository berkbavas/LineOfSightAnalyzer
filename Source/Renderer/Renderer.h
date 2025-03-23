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

        void OnMouseMoved(QMouseEvent* event);
        void OnKeyPressed(QKeyEvent* event);

      private:
        TerrainRenderer* mTerrainRenderer;
        LineOfSightRenderer* mLineOfSightRenderer;
        
        DEFINE_MEMBER_PTR(CameraManager, CameraManager);

        DEFINE_MEMBER(float, DevicePixelRatio, 1.0f);
    };
}