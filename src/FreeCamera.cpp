#include "FreeCamera.h"
#include "Helper.h"

#include <QtMath>

LineOfSightAnalyzer::FreeCamera::FreeCamera(QObject* parent)
    : Camera(parent)
    , mUpdateRotation(true)
    , mUpdatePosition(true)

{
    connect(this, &Camera::ActiveChanged, this, [=]() {
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

    SetVerticalFov(60.0f);
}

LineOfSightAnalyzer::FreeCamera::~FreeCamera() {}

void LineOfSightAnalyzer::FreeCamera::KeyPressed(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key)event->key(), true);
    mUpdatePosition = true;
}

void LineOfSightAnalyzer::FreeCamera::KeyReleased(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key)event->key(), false);
}

void LineOfSightAnalyzer::FreeCamera::MousePressed(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        mMouse.mX = event->pos().x();
        mMouse.mY = event->pos().y();
        mMouse.mPressed = true;
    }
}

void LineOfSightAnalyzer::FreeCamera::MouseReleased(QMouseEvent*)
{
    mMouse.mPressed = false;
}

void LineOfSightAnalyzer::FreeCamera::MouseMoved(QMouseEvent* event)
{
    if (mMouse.mPressed)
    {
        mMouse.mDx += mMouse.mX - event->pos().x();
        mMouse.mDy += mMouse.mY - event->pos().y();

        mMouse.mX = event->pos().x();
        mMouse.mY = event->pos().y();
        mUpdateRotation = true;
    }
}

void LineOfSightAnalyzer::FreeCamera::WheelMoved(QWheelEvent*) {}

void LineOfSightAnalyzer::FreeCamera::Update(float ifps)
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

QMatrix4x4 LineOfSightAnalyzer::FreeCamera::GetProjectionMatrix()
{
    QMatrix4x4 projection;
    projection.perspective(mVerticalFov, float(mWidth) / float(mHeight), mZNear, mZFar);
    return projection;
}

QVector3D LineOfSightAnalyzer::FreeCamera::GetEulerDegrees() const
{
    return Helper::GetEulerDegrees(mRotation);
}

const QMap<Qt::Key, QVector3D> LineOfSightAnalyzer::FreeCamera::KEY_BINDINGS = {
    {Qt::Key_W, QVector3D(0, 0, -1)},
    {Qt::Key_S, QVector3D(0, 0, 1)},
    {Qt::Key_A, QVector3D(-1, 0, 0)},
    {Qt::Key_D, QVector3D(1, 0, 0)},
    {Qt::Key_E, QVector3D(0, 1, 0)},
    {Qt::Key_Q, QVector3D(0, -1, 0)},
};