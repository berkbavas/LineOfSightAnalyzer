#ifndef TERRAIN_H
#define TERRAIN_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

#include "Common.h"

class Terrain : protected QOpenGLExtraFunctions
{
public:
    struct Vertex {
        QVector3D position;
        QVector2D texture;
    };

    Terrain(int width, int height);

    void render();

private:
    QVector<Vertex> mVertices;
    QVector<unsigned int> mIndices;

    DEFINE_MEMBER_CONST(int, Width);
    DEFINE_MEMBER_CONST(int, Height);
    DEFINE_MEMBER_CONST(float, WidthScale);
    DEFINE_MEMBER_CONST(float, HeightScale);

    // OpenGL Stuff
    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int mEBO;
};

#endif // TERRAIN_H
