#pragma once

#include "Core/Constants.h"
#include "Node/Camera/Camera.h"
#include "Node/Terrain.h"
#include "Renderer/Shader.h"
#include "Renderer/TextureLoader.h"

#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions_4_5_Core>

namespace LineOfSightAnalyzer
{
    class CameraManager;
    class LineOfSightRenderer;

    // Visualization enums
    enum class ColorScheme : int
    {
        GreenRed = 0,
        BlueYellow = 1,
        HeatMap = 2,
        PurpleCyan = 3
    };

    enum class TerrainColorMode : int
    {
        Texture = 0,
        HeightBased = 1,
        Grayscale = 2,
        HeightmapVis = 3
    };

    // Terrain analysis modes
    enum class TerrainAnalysisMode : int
    {
        None = 0,
        Slope = 1,           // Visualize terrain steepness
        Aspect = 2,          // Visualize terrain facing direction
        Curvature = 3        // Visualize terrain curvature (convex/concave)
    };

    class TerrainRenderer : protected QOpenGLFunctions_4_5_Core
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

        // Visualization settings
        ColorScheme mColorScheme{ ColorScheme::GreenRed };
        TerrainColorMode mTerrainColorMode{ TerrainColorMode::Texture };
        float mVisibilityOpacity{ 0.3f };
        bool mShowLOS{ true };
        bool mWireframeMode{ false };
        
        // Terrain analysis settings
        TerrainAnalysisMode mTerrainAnalysisMode{ TerrainAnalysisMode::None };
        float mSlopeThreshold{ 45.0f };  // Degrees - slopes above this are highlighted
        
        // Overlay settings
        bool mShowContourLines{ false };
        float mContourInterval{ 50.0f };  // Height interval between contour lines
        float mContourLineWidth{ 1.0f };
        
        bool mShowDistanceRings{ false };
        float mDistanceRingInterval{ 100.0f };  // Distance between rings
        
        bool mShowGrid{ false };
        float mGridSize{ 100.0f };
    };
}