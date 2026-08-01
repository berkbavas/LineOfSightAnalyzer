#pragma once

#include "Core/Texture.h"
#include "Util/Macros.h"

#include <memory>

#include <QOpenGLFunctions_4_5_Core>

namespace LineOfSightAnalyzer
{
    class Shader;

    class Terrain : protected QOpenGLFunctions_4_5_Core
    {
      public:
        Terrain(const Texture& Texture);
        ~Terrain();

        void Render();
        const Texture& GetTexture() const;

      private:
        QVector<float> mVertices;
        Texture mTexture;
        int mDensity{ 16 };
        int mWidth;
        int mHeight;

        // OpenGL Stuff
        unsigned int mVAO{ 0 };
        unsigned int mVBO{ 0 };

        DEFINE_MEMBER(float, MinElevation, 0);
        DEFINE_MEMBER(float, MaxElevation, 300);
    };

    using TerrainPtr = std::unique_ptr<Terrain>;
}
