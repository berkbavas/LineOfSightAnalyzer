#pragma once

#include "Node/Renderable.h"
#include "Structs/Texture.h"
#include "Structs/Vertex.h"

namespace LineOfSightAnalyzer
{
    class Terrain : public Renderable
    {
      public:
        Terrain(int width, int height, int density);

        void Construct() override;
        void Render() override;
        void Destroy() override;

      private:
        QVector<float> mVertices;
        int mDensity;
        int mWidth;
        int mHeight;

        DEFINE_MEMBER(float, MinElevation, 0);
        DEFINE_MEMBER(float, MaxElevation, 300);

        DEFINE_MEMBER(Texture, Texture);

        // OpenGL Stuff
        unsigned int mVAO{ 0 };
        unsigned int mVBO{ 0 };
    };
}
