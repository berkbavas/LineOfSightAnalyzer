#include "TextureLoader.h"

#include "Util/Logger.h"

#include <QImage>

LineOfSightAnalyzer::TextureLoader::TextureLoader()
{
    initializeOpenGLFunctions();
}

LineOfSightAnalyzer::Texture LineOfSightAnalyzer::TextureLoader::LoadTexture2D(const QString& Path)
{
    LOG_DEBUG("TextureLoader::LoadTexture2D: Loading texture at {}", Path.toStdString());

    QImage Image = QImage(Path);

    if (Image.isNull())
    {
        LOS_EXIT_FAILURE("TextureLoader::LoadTexture2D: Failed to load texture at {}", Path.toStdString());
    }

    Image = Image.convertToFormat(QImage::Format_RGBA8888);

    GLuint TextureId;
    glGenTextures(1, &TextureId);
    glBindTexture(GL_TEXTURE_2D, TextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Image.width(), Image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    LOG_DEBUG("TextureLoader::LoadTexture2D: Texture has been loaded.");

    Texture Texture;
    Texture.Id = TextureId;
    Texture.Target = GL_TEXTURE_2D;
    Texture.Width = Image.width();
    Texture.Height = Image.height();

    return Texture;
}