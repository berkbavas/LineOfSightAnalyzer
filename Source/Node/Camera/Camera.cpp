#include "Camera.h"

QMatrix4x4 LineOfSightAnalyzer::Camera::GetViewProjectionMatrix()
{
    return GetProjectionMatrix() * GetViewMatrix();
}

QMatrix4x4 LineOfSightAnalyzer::Camera::GetViewMatrix()
{
    QMatrix4x4 viewMatrix;
    viewMatrix.rotate(GetRotation().conjugated());
    viewMatrix.translate(-GetPosition());

    return viewMatrix;
}

QMatrix4x4 LineOfSightAnalyzer::Camera::GetRotationMatrix()
{
    auto rotation = GetViewMatrix();
    rotation.setColumn(3, QVector4D(0, 0, 0, 1));
    return rotation;
}

QVector3D LineOfSightAnalyzer::Camera::GetViewDirection()
{
    return GetRotation() * QVector3D(0, 0, -1);
}

void LineOfSightAnalyzer::Camera::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

float LineOfSightAnalyzer::Camera::GetAspectRatio() const
{
    return float(mWidth) / float(mHeight);
}