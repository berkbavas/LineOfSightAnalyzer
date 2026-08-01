#pragma once
    
#include <QOpenGLFunctions>
#include <QString>

namespace LineOfSightAnalyzer
{
    struct Texture
    {
        GLuint Id;     // OpenGL handle
        int Width;
        int Height;
    };
}