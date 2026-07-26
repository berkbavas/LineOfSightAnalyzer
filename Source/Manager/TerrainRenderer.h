#pragma once

#include "Camera/Camera.h"
#include "Core/Constants.h"
#include "Core/EventReceiver.h"
#include "Core/Framebuffer.h"
#include "Core/Shader.h"
#include "Terrain/Terrain.h"
#include "Util/TextureLoader.h"

#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions_4_5_Core>

namespace LineOfSightAnalyzer
{
    class Camera;
    class LineOfSightRenderer;

    class TerrainRenderer : protected QOpenGLFunctions_4_5_Core, public EventReceiver
    {
      public:
        TerrainRenderer();

        void Render(Camera* pActiveCamera, float DevicePixelRatio, float Ifps);
        void DrawGui();
        void Resize(int Width, int Height);
        bool OnMouseMoved(QMouseEvent* pEvent) override;
        void SetLineOfSightRenderer(LineOfSightRenderer* pLineOfSightAnalyzer);
        Framebuffer* GetFramebuffer() const;
        Terrain* GetTerrain() const;
        const QVector3D& GetMouseWorldPosition() const;
        void BlitToDefaultFramebuffer(int Width, int Height, float DevicePixelRatio);

      private:
        LineOfSightRenderer* mLineOfSightAnalyzer{ nullptr };

        QOpenGLFramebufferObjectFormat mFramebufferFormat;
        FramebufferPtr mFramebuffer{ nullptr };
        QVector<GLuint> mExtraColorAttachments;
        ShaderPtr mTerrainShader;
        TextureLoaderPtr mTextureLoader;
        TerrainPtr mTerrain{ nullptr };

        QVector3D mMouseWorldPosition{ 0.0f, 0.0f, 0.0f };

        float mBias{ 0.01f };

        // Visualization settings
        ColorScheme mColorScheme{ ColorScheme::GreenRed };
        TerrainColorMode mTerrainColorMode{ TerrainColorMode::Texture };
        float mVisibilityOpacity{ 0.3f };
        bool mShowLos{ true };
        bool mWireframeMode{ false };

        // Terrain analysis settings
        TerrainAnalysisMode mTerrainAnalysisMode{ TerrainAnalysisMode::None };
        float mSlopeThreshold{ 45.0f }; // Degrees - slopes above this are highlighted

        // Overlay settings
        bool mShowContourLines{ false };
        float mContourInterval{ 50.0f }; // Height interval between contour lines
        float mContourLineWidth{ 1.0f };

        bool mShowDistanceRings{ false };
        float mDistanceRingInterval{ 100.0f }; // Distance between rings

        bool mShowGrid{ false };
        float mGridSize{ 100.0f };
    };

    using TerrainRendererPtr = std::unique_ptr<TerrainRenderer>;
}