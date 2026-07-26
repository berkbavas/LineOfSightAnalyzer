#pragma once

#include "Camera/FreeCamera.h"
#include "Core/Enums.h"
#include "Core/EventReceiver.h"
#include "Core/Shader.h"
#include "Core/Texture.h"
#include "Terrain/Terrain.h"

#include <memory>

#include <QOpenGLFunctions_4_5_Core>
#include <QVector>

namespace LineOfSightAnalyzer
{
    class TerrainRenderer;

    class LineOfSightRenderer : protected QOpenGLFunctions_4_5_Core, public EventReceiver
    {
      public:
        LineOfSightRenderer();

        void Render(float Ifps);
        void DrawGui();
        void SetTerrainRenderer(TerrainRenderer* pTerrainRenderer);
        void SetTerrain(Terrain* pTerrain);
        bool OnKeyPressed(QKeyEvent* pEvent) override;

        QVector3D GetObserverPosition() const;

        // Analysis getters
        ObserverMode GetObserverMode() const { return mObserverMode; }
        float GetConeAngle() const { return mConeAngle; }
        float GetConeDirection() const { return mConeDirection; }
        float GetConePitch() const { return mConePitch; }
        float GetVerticalAngleMin() const { return mVerticalAngleMin; }
        float GetVerticalAngleMax() const { return mVerticalAngleMax; }

      private:
        void CreateObservers();
        GLuint CreateLineOfSightFramebuffer(int Width, int Height);

        ShaderPtr mObserverShader;

        TerrainRenderer* mTerrainRenderer;
        Terrain* mTerrain;

        float mMinLosDistance{ 10.0f };
        float mMaxLosDistance{ 1000.0f };

        GLuint mFramebuffer;

        float mObserverHeight{ 50 };

        // Observer mode settings
        ObserverMode mObserverMode{ ObserverMode::FullSphere };
        float mConeAngle{ 60.0f };         // Cone half-angle in degrees (for DirectionalCone mode)
        float mConeDirection{ 0.0f };      // Horizontal direction in degrees (0 = North, 90 = East)
        float mConePitch{ 0.0f };          // Vertical pitch in degrees (-90 to 90)
        float mVerticalAngleMin{ -90.0f }; // Min vertical angle (for Hemisphere: 0 to 90)
        float mVerticalAngleMax{ 90.0f };  // Max vertical angle

        DEFINE_MEMBER(bool, LockObserverPosition, false);
        DEFINE_MEMBER_CONST(std::vector<FreeCameraPtr>, Observers);
        DEFINE_MEMBER_CONST(Texture, DepthMap);

        static constexpr int NUMBER_OF_OBSERVERS{ 6 }; // Number of observers for full sphere coverage (6 for cube map)
    };

    using LineOfSightRendererPtr = std::unique_ptr<LineOfSightRenderer>;
}