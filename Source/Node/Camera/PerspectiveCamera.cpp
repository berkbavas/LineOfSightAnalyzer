#include "PerspectiveCamera.h"

const QMatrix4x4& LineOfSightAnalyzer::PerspectiveCamera::GetProjectionMatrix()
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.perspective(mVerticalFov, GetAspectRatio(), GetZNear(), GetZFar());
    return mProjectionMatrix;
}

float LineOfSightAnalyzer::PerspectiveCamera::GetHorizontalFov() const
{
    const auto hfov = qAtan(qTan(mVerticalFov / 2.0) / GetAspectRatio()) * 2.0f;

    return qAbs(qRadiansToDegrees(hfov));
}
