#include "Camera.h"

#include <QTransform>
#include <QtMath>

Camera::Camera(QObject *parent)
    : Node(parent)
    , mActive(false)
    , mWidth(1600)
    , mHeight(900)
    , mDevicePixelRatio(1.0f)
    , mZNear(1.0f)
    , mZFar(1000.0f)
{}

QMatrix4x4 Camera::getViewMatrix()
{
    QMatrix4x4 view;
    view.rotate(mRotation.conjugated());
    view.translate(-mPosition);

    return view;
}

QMatrix4x4 Camera::getViewProjectionMatrix()
{
    return getProjectionMatrix() * getViewMatrix();
}

QMatrix4x4 Camera::getRotationMatrix()
{
    auto rotation = getViewMatrix();
    rotation.setColumn(3, QVector4D(0, 0, 0, 1));
    return rotation;
}

void Camera::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

bool Camera::active() const
{
    return mActive;
}

void Camera::setActive(bool newActive)
{
    mActive = newActive;

    emit activeChanged();
}
