#include "Camera.h"

#include <QTransform>
#include <QtMath>

LineOfSightAnalyzer::Camera::Camera(QObject* parent)
    : Node(parent)
    , mActive(false)
    , mWidth(1600)
    , mHeight(900)
    , mDevicePixelRatio(1.0f)
    , mZNear(1.0f)
    , mZFar(1000.0f)
{}

QMatrix4x4 LineOfSightAnalyzer::Camera::GetViewMatrix()
{
    QMatrix4x4 view;
    view.rotate(mRotation.conjugated());
    view.translate(-mPosition);

    return view;
}

QMatrix4x4 LineOfSightAnalyzer::Camera::GetViewProjectionMatrix()
{
    return GetProjectionMatrix() * GetViewMatrix();
}

QMatrix4x4 LineOfSightAnalyzer::Camera::GetRotationMatrix()
{
    auto rotation = GetViewMatrix();
    rotation.setColumn(3, QVector4D(0, 0, 0, 1));
    return rotation;
}

QVector3D LineOfSightAnalyzer::Camera::GetViewDirection()
{
    return mRotation * QVector3D(0, 0, -1);
}

void LineOfSightAnalyzer::Camera::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

bool LineOfSightAnalyzer::Camera::GetActive() const
{
    return mActive;
}

void LineOfSightAnalyzer::Camera::SetActive(bool newActive)
{
    if (mActive == newActive)
        return;

    mActive = newActive;
    emit ActiveChanged();
}