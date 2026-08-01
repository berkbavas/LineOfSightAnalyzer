#include "Terrain.h"

#include "Core/Constants.h"

LineOfSightAnalyzer::Terrain::Terrain(const Texture &Texture)
    : mTexture(Texture)
    , mWidth(Texture.Width)
    , mHeight(Texture.Height)
{
    for (unsigned I = 0; I <= mDensity - 1; I++)
    {
        for (unsigned J = 0; J <= mDensity - 1; J++)
        {
            mVertices.push_back(-mWidth / 2.0f + mWidth * I / (float) mDensity);   // v.x
            mVertices.push_back(0.0f);                                             // v.y
            mVertices.push_back(-mHeight / 2.0f + mHeight * J / (float) mDensity); // v.z
            mVertices.push_back(I / (float) mDensity);                             // u
            mVertices.push_back(J / (float) mDensity);                             // v

            mVertices.push_back(-mWidth / 2.0f + mWidth * (I + 1) / (float) mDensity); // v.x
            mVertices.push_back(0.0f);                                                 // v.y
            mVertices.push_back(-mHeight / 2.0f + mHeight * J / (float) mDensity);     // v.z
            mVertices.push_back((I + 1) / (float) mDensity);                           // u
            mVertices.push_back(J / (float) mDensity);                                 // v

            mVertices.push_back(-mWidth / 2.0f + mWidth * I / (float) mDensity);         // v.x
            mVertices.push_back(0.0f);                                                   // v.y
            mVertices.push_back(-mHeight / 2.0f + mHeight * (J + 1) / (float) mDensity); // v.z
            mVertices.push_back(I / (float) mDensity);                                   // u
            mVertices.push_back((J + 1) / (float) mDensity);                             // v

            mVertices.push_back(-mWidth / 2.0f + mWidth * (I + 1) / (float) mDensity);   // v.x
            mVertices.push_back(0.0f);                                                   // v.y
            mVertices.push_back(-mHeight / 2.0f + mHeight * (J + 1) / (float) mDensity); // v.z
            mVertices.push_back((I + 1) / (float) mDensity);                             // u
            mVertices.push_back((J + 1) / (float) mDensity);                             // v
        }
    }

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), mVertices.constData(), GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    // Unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Clear the vertices vector to free up memory
    mVertices = {};
}

LineOfSightAnalyzer::Terrain::~Terrain()
{
    if (mVAO)
    {
        glDeleteVertexArrays(1, &mVAO);
        mVAO = 0;
    }

    if (mVBO)
    {
        glDeleteBuffers(1, &mVBO);
        mVBO = 0;
    }
}

void LineOfSightAnalyzer::Terrain::Render()
{

    glBindVertexArray(mVAO);
    glDrawArrays(GL_PATCHES, 0, 4 * mDensity * mDensity);
    glBindVertexArray(0);
}

const LineOfSightAnalyzer::Texture &LineOfSightAnalyzer::Terrain::GetTexture() const
{
    return mTexture;
}
