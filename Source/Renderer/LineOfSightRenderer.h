#pragma once

#include "Node/Terrain.h"
#include "Renderer/Shader.h"

#include <QOpenGLFunctions_4_5_Core>
#include <QVector>

namespace LineOfSightAnalyzer
{
    class FreeCamera;
    class TerrainRenderer;

    class LineOfSightRenderer : protected QOpenGLFunctions_4_5_Core
    {
      public:
        LineOfSightRenderer() = default;

        void Init();
        void Render(float ifps);
        void DrawGui();
        void SetTerrainRenderer(TerrainRenderer* terrainRenderer);
        
        QVector3D GetObserverPosition() const;

      private:
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
    };

}