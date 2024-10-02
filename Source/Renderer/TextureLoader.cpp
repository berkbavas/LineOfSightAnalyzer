#include "TextureLoader.h"

#include "Core/FailureBehaviour.h"
#include "Util/Logger.h"

#include <QImage>

LineOfSightAnalyzer::TextureLoader::TextureLoader()
{
    initializeOpenGLFunctions();
}

LineOfSightAnalyzer::Texture LineOfSightAnalyzer::TextureLoader::LoadTexture2D(const QString& path)
{
    LOG_DEBUG("TextureLoader::LoadTexture2D: Loading texture at {}", path.toStdString());

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    QImage image = QImage(path);

    if (image.isNull())
    {
        LOG_FATAL("TextureLoader::LoadTexture2D: Image is null.");
        FailureBehaviour::Failure(FailureType::COULD_NOT_LOAD_TEXTURE);
    }

    image = image.convertToFormat(QImage::Format_RGBA8888);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    LOG_DEBUG("TextureLoader::LoadTexture2D: Texture has been loaded.");

    Texture texture;
    texture.id = textureId;
    texture.target = GL_TEXTURE_2D;
    texture.width = image.width();
    texture.height = image.height();

    return texture;
}