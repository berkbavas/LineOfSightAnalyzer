#include "Terrain.h"

#include "Constants.h"

Terrain::Terrain(int width, int height)
    : mWidth(width)
    , mHeight(height)
    , mWidthScale(HEIGHT_MAP_WIDTH_SCALE)
    , mHeightScale(HEIGHT_MAP_HEIGHT_SCALE)
{
    // Vertices
    for (int i = 0; i < mHeight; i++)
    {
        for (int j = 0; j < mWidth; j++)
        {
            float x = j * mWidthScale - 0.5f * mWidth * mWidthScale;
            float y = 0;
            float z = i * mHeightScale - 0.5f * mHeight * mHeightScale;
            float u = j / float(mWidth - 1);
            float v = i / float(mHeight - 1);

            Vertex vertex;
            vertex.position = QVector3D(x, y, z);
            vertex.texture = QVector2D(u, v);
            mVertices << vertex;
        }
    }

    // Indices
    for (int i = 0; i < mHeight - 1; i++)
    {
        for (int j = 0; j < mWidth - 1; j++)
        {
            mIndices << mWidth * i + j;
            mIndices << mWidth * (i + 1) + j;
            mIndices << mWidth * i + j + 1;

            mIndices << mWidth * (i + 1) + j;
            mIndices << mWidth * (i + 1) + j + 1;
            mIndices << mWidth * i + j + 1;
        }
    }

    // OpenGL Stuff
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.constData(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.constData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture));
    glEnableVertexAttribArray(1);
}

void Terrain::render()
{
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
