#pragma once

#include "Camera/DummyCamera.h"
#include "Core/CubicFramebuffer.h"
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
        void SetTerrain(Terrain* pTerrain);

        ObserverMode GetObserverMode() const { return mObserverMode; }
        float GetConeAngle() const { return mConeAngle; }
        float GetConeDirection() const { return mConeDirection; }
        float GetConePitch() const { return mConePitch; }
        float GetVerticalAngleMin() const { return mVerticalAngleMin; }
        float GetVerticalAngleMax() const { return mVerticalAngleMax; }
        const QVector3D& GetObserverPositionOnTerrain() const { return mObserverPositionOnTerrain; }
        float GetMaxLosDistance() const { return mMaxLosDistance; }
        float GetMinLosDistance() const { return mMinLosDistance; }
        QVector3D GetObserverPosition() const;
        GLuint GetDepthMap() const;

        void SetObserverPositionOnTerrain(const QVector3D& Position);

      private:
        void CreateObservers();
        void UpdateObservers();

        CubicFramebufferPtr mObserverFramebuffer{ nullptr };
        ShaderPtr mObserverShader;
        Terrain* mTerrain;

        float mMinLosDistance{ 10.0f };
        float mMaxLosDistance{ 1000.0f };
        QVector3D mObserverPositionOnTerrain{ 0.0f, 0.0f, 0.0f };
        float mObserverHeight{ 50 };

        // Observer mode settings
        ObserverMode mObserverMode{ ObserverMode::FullSphere };
        float mConeAngle{ 60.0f };         // Cone half-angle in degrees (for DirectionalCone mode)
        float mConeDirection{ 0.0f };      // Horizontal direction in degrees (0 = North, 90 = East)
        float mConePitch{ 0.0f };          // Vertical pitch in degrees (-90 to 90)
        float mVerticalAngleMin{ -90.0f }; // Min vertical angle (for Hemisphere: 0 to 90)
        float mVerticalAngleMax{ 90.0f };  // Max vertical angle

        std::vector<DummyCameraPtr> mObservers;

        int mFramebufferWidth{ 4096 };
        int mFramebufferHeight{ 4096 };
    };

    using LineOfSightRendererPtr = std::unique_ptr<LineOfSightRenderer>;
}