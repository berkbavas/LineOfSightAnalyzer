#include "PerspectiveCamera.h"

QMatrix4x4 LineOfSightAnalyzer::PerspectiveCamera::GetProjectionMatrix() const
{
    QMatrix4x4 ProjectionMatrix;
    ProjectionMatrix.perspective(mVerticalFov, GetAspectRatio(), mZNear, mZFar);
    return ProjectionMatrix;
}

float LineOfSightAnalyzer::PerspectiveCamera::GetHorizontalFov() const
{
    float AspectRatio = GetAspectRatio();
    float VerticalFovRad = qDegreesToRadians(mVerticalFov);
    float HorizontalFovRad = 2.0f * std::atan(std::tan(VerticalFovRad / 2.0f) * AspectRatio);
    return qRadiansToDegrees(HorizontalFovRad);
}

float LineOfSightAnalyzer::PerspectiveCamera::GetAspectRatio() const
{
    return float(mWidth) / float(mHeight);
}

float LineOfSightAnalyzer::PerspectiveCamera::GetVerticalFov() const
{
    return mVerticalFov;
}

void LineOfSightAnalyzer::PerspectiveCamera::SetVerticalFov(float VerticalFov)
{
    mVerticalFov = VerticalFov;
}
