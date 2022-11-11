#include "FreeCamera.h"
#include "Helper.h"

#include <QtMath>

FreeCamera::FreeCamera(QObject *parent)
    : PerspectiveCamera(parent)
    , mUpdateRotation(true)
    , mUpdatePosition(true)

{
    connect(this, &Camera::activeChanged, this, [=]() {
        if (!mActive)
        {
            auto keys = mPressedKeys.keys();
            for (auto key : keys)
            {
                mPressedKeys.insert(key, false);
            }

            mMouse.mPressed = false;
        }
    });
}

FreeCamera::~FreeCamera() {}

void FreeCamera::keyPressed(QKeyEvent *event)
{
    mPressedKeys.insert((Qt::Key) event->key(), true);
    mUpdatePosition = true;
}

void FreeCamera::keyReleased(QKeyEvent *event)
{
    mPressedKeys.insert((Qt::Key) event->key(), false);
}

void FreeCamera::mousePressed(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        mMouse.mX = event->position().x();
        mMouse.mY = event->position().y();
        mMouse.mPressed = true;
    }
}

void FreeCamera::mouseReleased(QMouseEvent *)
{
    mMouse.mPressed = false;
}

void FreeCamera::mouseMoved(QMouseEvent *event)
{
    if (mMouse.mPressed)
    {
        mMouse.mDx += mMouse.mX - event->position().x();
        mMouse.mDy += mMouse.mY - event->position().y();

        mMouse.mX = event->position().x();
        mMouse.mY = event->position().y();
        mUpdateRotation = true;
    }
}

void FreeCamera::wheelMoved(QWheelEvent *) {}

void FreeCamera::update(float ifps)
{
    // Rotation
    if (mUpdateRotation)
    {
        mRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 30.0f * mMouse.mDx * ifps) * mRotation;
        mRotation = mRotation * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 30.0f * mMouse.mDy * ifps);

        mMouse.mDy = 0.0f;
        mMouse.mDx = 0.0f;
        mUpdateRotation = false;
    }

    // Translation
    if (mUpdatePosition)
    {
        const QList<Qt::Key> keys = mPressedKeys.keys();

        float speed = 25.0f;

        if (mPressedKeys[Qt::Key_Control])
            speed = 1000.0f;
        else if (mPressedKeys[Qt::Key_Shift])
            speed = 100.0f;

        for (auto key : keys)
            if (mPressedKeys.value(key, false))
                mPosition += speed * ifps * mRotation.rotatedVector(KEY_BINDINGS.value(key, QVector3D(0, 0, 0)));
    }

    if (mPressedKeys.empty())
        mUpdatePosition = false;
}

QVector3D FreeCamera::getEulerDegrees() const
{
    return Helper::getEulerDegrees(mRotation);
}

const QMap<Qt::Key, QVector3D> FreeCamera::KEY_BINDINGS = {
    {Qt::Key_W, QVector3D(0, 0, -1)},
    {Qt::Key_S, QVector3D(0, 0, 1)},
    {Qt::Key_A, QVector3D(-1, 0, 0)},
    {Qt::Key_D, QVector3D(1, 0, 0)},
    {Qt::Key_E, QVector3D(0, 1, 0)},
    {Qt::Key_Q, QVector3D(0, -1, 0)},
};
