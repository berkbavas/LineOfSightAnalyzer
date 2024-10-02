#include "OrthographicCamera.h"

void LineOfSightAnalyzer::OrthographicCamera::Update(float ifps)
{
    if (mUpdatePosition)
    {
        mLeft -= mDevicePixelRatio * mZoom * mMouse.dx;
        mTop += mDevicePixelRatio * mZoom * mMouse.dy;
        mMouse.dx = 0;
        mMouse.dy = 0;
        mUpdatePosition = false;
    }
}

void LineOfSightAnalyzer::OrthographicCamera::OnMousePressed(QMouseEvent* event)
{
    mMouse.x = event->pos().x();
    mMouse.y = event->pos().y();
    mMouse.dx = 0;
    mMouse.dy = 0;
    mMouse.button = event->button();
}

void LineOfSightAnalyzer::OrthographicCamera::OnMouseReleased(QMouseEvent*)
{
    mMouse.button = Qt::NoButton;
}

void LineOfSightAnalyzer::OrthographicCamera::OnMouseMoved(QMouseEvent* event)
{
    if (mMouse.button == mActionReceiveButton)
    {
        mMouse.dx += mMouse.x - event->pos().x();
        mMouse.dy += mMouse.y - event->pos().y();

        mMouse.x = event->pos().x();
        mMouse.y = event->pos().y();
        mUpdatePosition = true;
    }
}

void LineOfSightAnalyzer::OrthographicCamera::OnWheelMoved(QWheelEvent* event)
{
    if (event->angleDelta().y() < 0)
    {
        mZoom += mZoom / 64;
    }
    else
    {
        mZoom -= mZoom / 64;
    }

    mZoom = qMax(1.0f, qMin(128.0f, mZoom));
}

const QMatrix4x4& LineOfSightAnalyzer::OrthographicCamera::GetProjectionMatrix()
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.ortho(-mZoom * mWidth / 2 - mLeft, //
                            mZoom * mWidth / 2 - mLeft,
                            -mZoom * mHeight / 2 - mTop,
                            mZoom * mHeight / 2 - mTop,
                            mZNear,
                            mZFar);
    return mProjectionMatrix;
}
