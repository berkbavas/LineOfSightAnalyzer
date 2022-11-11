#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(QObject *parent)
    : Camera(parent)
{
    setVerticalFov(60.0f);
}

void PerspectiveCamera::setHorizontalFov(float newHorizontalFov)
{
    mHorizontalFov = newHorizontalFov;

    float halfHorizontalFovRadian = 0.5f * qDegreesToRadians(mHorizontalFov);
    float verticalFovRadian = 2 * atan(tan(halfHorizontalFovRadian) * (mWidth / float(mHeight)));

    mVerticalFov = qRadiansToDegrees(verticalFovRadian);
}

void PerspectiveCamera::setVerticalFov(float newVerticalFov)
{
    mVerticalFov = newVerticalFov;

    float halfVerticalFovRadian = 0.5f * qDegreesToRadians(mVerticalFov);
    float horizontalFovRadian = 2 * atan(tan(halfVerticalFovRadian) / (mWidth / float(mHeight)));

    mHorizontalFov = qRadiansToDegrees(horizontalFovRadian);
}

float PerspectiveCamera::verticalFov() const
{
    return mVerticalFov;
}

float PerspectiveCamera::horizontalFov() const
{
    return mHorizontalFov;
}

QMatrix4x4 PerspectiveCamera::getProjectionMatrix()
{
    QMatrix4x4 projection;
    projection.perspective(mVerticalFov, mWidth / float(mHeight), mZNear, mZFar);
    return projection;
}
