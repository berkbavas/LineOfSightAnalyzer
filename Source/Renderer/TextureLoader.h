#pragma once

#include "Structs/Texture.h"

#include <QOpenGLExtraFunctions>


namespace LineOfSightAnalyzer
{
    class TextureLoader : protected QOpenGLExtraFunctions
    {
      public:
        TextureLoader();

        Texture LoadTexture2D(const QString& path);
    };
}