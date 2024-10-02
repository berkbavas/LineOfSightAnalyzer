#pragma once

#include "Core/Constants.h"
#include "Node/Camera/Camera.h"
#include "Node/Terrain.h"
#include "Renderer/Shader.h"
#include "Renderer/TextureLoader.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

namespace LineOfSightAnalyzer
{
    class CameraManager;
    class LineOfSightRenderer;

    class TerrainRenderer : protected QOpenGLExtraFunctions
    {
        DISABLE_COPY(TerrainRenderer);

      public:
        TerrainRenderer() = default;

        void Init();
        void Render(float ifps);
        void DrawGui();
        void Resize(int w, int h);

        void onMouseMoved(QMouseEvent* event);

        void SetLineOfSightRenderer(LineOfSightRenderer* analyzer) { mLineOfSightAnalyzer = analyzer; }

      private:
        void CreateFramebuffer();
        void DeleteFramebuffer();

        LineOfSightRenderer* mLineOfSightAnalyzer;

        QOpenGLFramebufferObjectFormat mFramebufferFormat;
        QOpenGLFramebufferObject* mFramebuffer{ nullptr };
        Shader* mTerrainShader;

        TextureLoader* mTextureLoader;

        DEFINE_MEMBER(QVector3D, MouseWorldPosition);
        DEFINE_MEMBER_PTR(CameraManager, CameraManager);

        DEFINE_MEMBER_PTR(Camera, ActiveCamera);
        DEFINE_MEMBER_PTR(Terrain, Terrain);

        DEFINE_MEMBER(float, Width, INITIAL_WIDTH);
        DEFINE_MEMBER(float, Height, INITIAL_HEIGHT);
        DEFINE_MEMBER(float, DevicePixelRatio, 1.0f);

        float mBias{ 0.01f };
    };
}