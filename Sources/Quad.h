#pragma once

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

namespace LineOfSightAnalyzer
{
    class Quad : protected QOpenGLExtraFunctions
    {
    public:
        Quad();

        void render();

    private:
        unsigned int mVAO;
        unsigned int mVBO;
        static const float VERTICES[24];
    };
}
