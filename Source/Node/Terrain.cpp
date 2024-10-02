#include "Terrain.h"

#include "Core/Constants.h"

LineOfSightAnalyzer::Terrain::Terrain(int width, int height, int density)
    : mWidth(width)
    , mHeight(height)
    , mDensity(density)
{
    for (unsigned i = 0; i <= density - 1; i++)
    {
        for (unsigned j = 0; j <= density - 1; j++)
        {
            mVertices.push_back(-width / 2.0f + width * i / (float) density);   // v.x
            mVertices.push_back(0.0f);                                          // v.y
            mVertices.push_back(-height / 2.0f + height * j / (float) density); // v.z
            mVertices.push_back(i / (float) density);                           // u
            mVertices.push_back(j / (float) density);                           // v

            mVertices.push_back(-width / 2.0f + width * (i + 1) / (float) density); // v.x
            mVertices.push_back(0.0f);                                              // v.y
            mVertices.push_back(-height / 2.0f + height * j / (float) density);     // v.z
            mVertices.push_back((i + 1) / (float) density);                         // u
            mVertices.push_back(j / (float) density);                               // v

            mVertices.push_back(-width / 2.0f + width * i / (float) density);         // v.x
            mVertices.push_back(0.0f);                                                // v.y
            mVertices.push_back(-height / 2.0f + height * (j + 1) / (float) density); // v.z
            mVertices.push_back(i / (float) density);                                 // u
            mVertices.push_back((j + 1) / (float) density);                           // v

            mVertices.push_back(-width / 2.0f + width * (i + 1) / (float) density);   // v.x
            mVertices.push_back(0.0f);                                                // v.y
            mVertices.push_back(-height / 2.0f + height * (j + 1) / (float) density); // v.z
            mVertices.push_back((i + 1) / (float) density);                           // u
            mVertices.push_back((j + 1) / (float) density);                           // v
        }
    }
}

void LineOfSightAnalyzer::Terrain::Construct()
{
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.constData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void LineOfSightAnalyzer::Terrain::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_PATCHES, 0, 4 * mDensity * mDensity);
}

void LineOfSightAnalyzer::Terrain::Destroy()
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