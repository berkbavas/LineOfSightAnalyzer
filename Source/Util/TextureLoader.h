#pragma once

#include "Core/Texture.h"

#include <memory>

#include <QOpenGLExtraFunctions>
#include <QString>

namespace LineOfSightAnalyzer
{
    class TextureLoader : protected QOpenGLExtraFunctions
    {
      public:
        TextureLoader();

        Texture LoadTexture2D(const QString& Path);
    };

    using TextureLoaderPtr = std::unique_ptr<TextureLoader>;

}