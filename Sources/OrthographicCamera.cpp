#include "OrthographicCamera.h"
#include "Constants.h"

LineOfSightAnalyzer::OrthographicCamera::OrthographicCamera(QObject* parent)
    : Camera(parent)
    , mZoom(DEFUALT_ZOOM)
    , mLeft(0)
    , mTop(0)
    , mUpdatePosition(false)
    , mTimeElapsed(0)

{
    connect(this, &Camera::ActiveChanged, this, [=]() {
        if (!mActive)
        {
            // Reset to defaults
            mZoom = DEFUALT_ZOOM;
            mLeft = 0.0f;
            mTop = 0.0f;
        }
        });
}

void LineOfSightAnalyzer::OrthographicCamera::KeyPressed(QKeyEvent*) {}

void LineOfSightAnalyzer::OrthographicCamera::KeyReleased(QKeyEvent*) {}

void LineOfSightAnalyzer::OrthographicCamera::MousePressed(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        mMouse.mX = event->pos().x();
        mMouse.mY = event->pos().y();
        mMouse.mPressed = true;
    }
}

void LineOfSightAnalyzer::OrthographicCamera::MouseReleased(QMouseEvent*)
{
    mMouse.mPressed = false;
}

void LineOfSightAnalyzer::OrthographicCamera::MouseMoved(QMouseEvent* event)
{
    if (mMouse.mPressed)
    {
        mMouse.mDx += mMouse.mX - event->pos().x();
        mMouse.mDy += mMouse.mY - event->pos().y();

        mMouse.mX = event->pos().x();
        mMouse.mY = event->pos().y();
        mUpdatePosition = true;
    }
}

void LineOfSightAnalyzer::OrthographicCamera::WheelMoved(QWheelEvent* event)
{
    if (event->angleDelta().y() < 0)
        mZoom = 1.1f * mZoom;
    else
        mZoom = mZoom / 1.1f;

    mZoom = qMax(1.0f, qMin(100.0f, mZoom));
}

void LineOfSightAnalyzer::OrthographicCamera::Update(float ifps)
{
    if (mUpdatePosition)
    {
        mLeft -= mDevicePixelRatio * mZoom * mMouse.mDx;
        mTop += mDevicePixelRatio * mZoom * mMouse.mDy;
        mMouse.mDx = 0;
        mMouse.mDy = 0;
        mUpdatePosition = false;
    }
}

QMatrix4x4 LineOfSightAnalyzer::OrthographicCamera::GetProjectionMatrix()
{
    QMatrix4x4 projection;
    projection.ortho(-mZoom * mWidth / 2 - mLeft, //
        mZoom * mWidth / 2 - mLeft,
        -mZoom * mHeight / 2 - mTop,
        mZoom * mHeight / 2 - mTop,
        mZNear,
        mZFar);
    return projection;
}