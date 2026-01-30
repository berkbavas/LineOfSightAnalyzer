#pragma once

#include "Node/Terrain.h"
#include "Renderer/Shader.h"

#include <QOpenGLFunctions_4_5_Core>
#include <QVector>

namespace LineOfSightAnalyzer
{
    class FreeCamera;
    class TerrainRenderer;

    // Observer mode for different LOS analysis types
    enum class ObserverMode : int
    {
        FullSphere = 0,      // 360° horizontal, 180° vertical (default)
        Hemisphere = 1,      // 360° horizontal, 90° vertical (only looks up/around, not below)
        DirectionalCone = 2  // Limited FOV cone in a specific direction
    };

    class LineOfSightRenderer : protected QOpenGLFunctions_4_5_Core
    {
      public:
        LineOfSightRenderer() = default;

        void Init();
        void Render(float ifps);
        void DrawGui();
        void SetTerrainRenderer(TerrainRenderer* terrainRenderer);
        
        QVector3D GetObserverPosition() const;
        
        // Analysis getters
        float GetVisibleAreaPercentage() const { return mVisibleAreaPercentage; }
        float GetAverageVisibleDistance() const { return mAverageVisibleDistance; }
        ObserverMode GetObserverMode() const { return mObserverMode; }
        float GetConeAngle() const { return mConeAngle; }
        float GetConeDirection() const { return mConeDirection; }
        float GetConePitch() const { return mConePitch; }
        float GetVerticalAngleMin() const { return mVerticalAngleMin; }
        float GetVerticalAngleMax() const { return mVerticalAngleMax; }

      private:
        void UpdateAnalysisStatistics();
        
        Shader* mObserverShader;
        TerrainRenderer* mTerrainRenderer;
        Terrain* mTerrain;

        DEFINE_MEMBER_CONST(QVector<FreeCamera*>, Observers);
        DEFINE_MEMBER_CONST(Texture, DepthMap);

        float mMinLosDistance{ 10.0f };
        float mMaxLosDistance{ 1000.0f };

        GLuint mFramebuffer;

        float mObserverHeight{ 50 };

        DEFINE_MEMBER(bool, LockObserverPosition, false);
        
        // Observer mode settings
        ObserverMode mObserverMode{ ObserverMode::FullSphere };
        float mConeAngle{ 60.0f };        // Cone half-angle in degrees (for DirectionalCone mode)
        float mConeDirection{ 0.0f };     // Horizontal direction in degrees (0 = North, 90 = East)
        float mConePitch{ 0.0f };         // Vertical pitch in degrees (-90 to 90)
        float mVerticalAngleMin{ -90.0f }; // Min vertical angle (for Hemisphere: 0 to 90)
        float mVerticalAngleMax{ 90.0f };  // Max vertical angle
        
        // Analysis statistics
        float mVisibleAreaPercentage{ 0.0f };
        float mAverageVisibleDistance{ 0.0f };
        int mVisiblePixelCount{ 0 };
        int mTotalAnalyzedPixels{ 0 };
        
        // Distance band analysis
        float mDistanceBand1Percentage{ 0.0f }; // 0-25% of max distance
        float mDistanceBand2Percentage{ 0.0f }; // 25-50%
        float mDistanceBand3Percentage{ 0.0f }; // 50-75%
        float mDistanceBand4Percentage{ 0.0f }; // 75-100%
        
        bool mShowStatistics{ true };
    };

}