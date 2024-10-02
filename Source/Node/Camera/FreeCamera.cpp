#include "FreeCamera.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

void LineOfSightAnalyzer::FreeCamera::Update(float ifps)
{
    // Rotation
    if (mUpdateRotation)
    {
        RotateGlobal(QVector3D(0, 1, 0), mAngularSpeedMultiplier * mAngularSpeed * mMouse.dx * ifps);
        RotateLocal(QVector3D(1, 0, 0), mAngularSpeedMultiplier * mAngularSpeed * mMouse.dy * ifps);
        mMouse.dx = 0.0f;
        mMouse.dy = 0.0f;
        mUpdateRotation = false;
    }

    // Translation
    if (mUpdatePosition)
    {
        bool anyKeyPressed = false;

        const auto keys = mPressedKeys.keys();

        for (const auto key : keys)
        {
            if (mPressedKeys.value(key, false))
            {
                const auto binding = KEY_BINDINGS.value(key, QVector3D(0, 0, 0));
                const auto direction = GetRotation().rotatedVector(binding);
                const auto delta = mLinearSpeed * mLinearSpeedMultiplier * ifps * direction;
                Translate(delta);
                anyKeyPressed = true;
            }
        }

        if (mPressedKeys[Qt::Key_Control])
        {
            mLinearSpeed = 1000.0f;
        }
        else if (mPressedKeys[Qt::Key_Shift])
        {
            mLinearSpeed = 250.0f;
        }
        else if (anyKeyPressed)
        {
            mLinearSpeed += 100 * ifps;
        }
        else
        {
            mLinearSpeed = 50.0f;
        }

        mLinearSpeed = qBound(50.0f, mLinearSpeed, 1000.0f);
    }

    if (mPressedKeys.empty())
    {
        mUpdatePosition = false;
    }
}

void LineOfSightAnalyzer::FreeCamera::Reset()
{
    const auto keys = mPressedKeys.keys();

    for (const auto key : keys)
    {
        mPressedKeys.insert(key, false);
    }

    mUpdatePosition = false;
    mUpdateRotation = false;
    mMouse.Reset();
}

void LineOfSightAnalyzer::FreeCamera::OnKeyPressed(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key) event->key(), true);
    mUpdatePosition = true;
}

void LineOfSightAnalyzer::FreeCamera::OnKeyReleased(QKeyEvent* event)
{
    mPressedKeys.insert((Qt::Key) event->key(), false);
}

void LineOfSightAnalyzer::FreeCamera::OnMousePressed(QMouseEvent* event)
{
    mMouse.x = event->pos().x();
    mMouse.y = event->pos().y();
    mMouse.button = event->button();
}

void LineOfSightAnalyzer::FreeCamera::OnMouseReleased(QMouseEvent* event)
{
    if (mMouse.button == event->button())
    {
        mMouse.button = Qt::NoButton;
    }
}

void LineOfSightAnalyzer::FreeCamera::OnMouseMoved(QMouseEvent* event)
{
    if (mMouse.button == mActionReceiveButton)
    {
        mMouse.dx += mMouse.x - event->pos().x();
        mMouse.dy += mMouse.y - event->pos().y();

        mMouse.x = event->pos().x();
        mMouse.y = event->pos().y();

        mUpdateRotation = true;
    }
}

const QMap<Qt::Key, QVector3D> LineOfSightAnalyzer::FreeCamera::KEY_BINDINGS = //
    {
        { Qt::Key_W, QVector3D(0, 0, -1) },
        { Qt::Key_S, QVector3D(0, 0, 1) },
        { Qt::Key_A, QVector3D(-1, 0, 0) },
        { Qt::Key_D, QVector3D(1, 0, 0) },
        { Qt::Key_E, QVector3D(0, 1, 0) },
        { Qt::Key_Q, QVector3D(0, -1, 0) }
    };
