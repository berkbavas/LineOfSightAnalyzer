#include "PersecutorCamera.h"

void LineOfSightAnalyzer::PersecutorCamera::Update(float ifps)
{
    if (mTarget)
    {
        mYaw += mAngularSpeedMultiplier * mAngularSpeed * mMouse.dx * ifps;
        mPitch += mAngularSpeedMultiplier * mAngularSpeed * mMouse.dy * ifps;

        auto newRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mYaw) * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mPitch);
        auto newWorldPosition = mTarget->GetPosition() + mDistance * newRotation * QVector3D(0, 0, 1);

        SetPosition(newWorldPosition);
        SetRotation(newRotation);

        mMouse.dx = 0.0f;
        mMouse.dy = 0.0f;
    }
}

void LineOfSightAnalyzer::PersecutorCamera::Reset()
{
    mDistance = 10.0f;
    mYaw = 0.0f;
    mPitch = 0.0f;
}

void LineOfSightAnalyzer::PersecutorCamera::OnMousePressed(QMouseEvent* event)
{
    mMouse.x = event->pos().x();
    mMouse.y = event->pos().y();
    mMouse.button = event->button();
}

void LineOfSightAnalyzer::PersecutorCamera::OnMouseReleased(QMouseEvent* event)
{
    if (mMouse.button == event->button())
    {
        mMouse.Reset();
    }
}

void LineOfSightAnalyzer::PersecutorCamera::OnMouseMoved(QMouseEvent* event)
{
    if (mMouse.button == mActionReceiveButton)
    {
        mMouse.dx += mMouse.x - event->pos().x();
        mMouse.dy += mMouse.y - event->pos().y();

        mMouse.x = event->pos().x();
        mMouse.y = event->pos().y();
    }
}

void LineOfSightAnalyzer::PersecutorCamera::OnWheelMoved(QWheelEvent* event)
{
    if (event->angleDelta().y() < 0)
    {
        mDistance += mWheelStep;
    }

    if (event->angleDelta().y() > 0)
    {
        mDistance -= mWheelStep;
    }

    mDistance = qBound(mMinimumDistance, mDistance, mMaximumDistance);
}

void LineOfSightAnalyzer::PersecutorCamera::SetTarget(Node* node)
{
    mTarget = node;
    Reset();
    Update(0);
}
