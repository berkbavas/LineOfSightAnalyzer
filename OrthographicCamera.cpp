#include "OrthographicCamera.h"
#include "Constants.h"

OrthographicCamera::OrthographicCamera(QObject *parent)
    : Camera(parent)
    , mZoom(DEFUALT_ZOOM)
    , mLeft(0)
    , mTop(0)
    , mUpdatePosition(false)
    , mTimeElapsed(0)

{
    connect(this, &Camera::activeChanged, this, [=]() {
        if (!mActive)
        {
            // Reset to defaults
            mZoom = DEFUALT_ZOOM;
            mLeft = 0.0f;
            mTop = 0.0f;
        }
    });
}

void OrthographicCamera::keyPressed(QKeyEvent *) {}

void OrthographicCamera::keyReleased(QKeyEvent *) {}

void OrthographicCamera::mousePressed(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        mMouse.mX = event->position().x();
        mMouse.mY = event->position().y();
        mMouse.mPressed = true;
    }
}

void OrthographicCamera::mouseReleased(QMouseEvent *)
{
    mMouse.mPressed = false;
}

void OrthographicCamera::mouseMoved(QMouseEvent *event)
{
    if (mMouse.mPressed)
    {
        mMouse.mDx += mMouse.mX - event->position().x();
        mMouse.mDy += mMouse.mY - event->position().y();

        mMouse.mX = event->position().x();
        mMouse.mY = event->position().y();
        mUpdatePosition = true;
    }
}

void OrthographicCamera::wheelMoved(QWheelEvent *event)
{
    if (event->angleDelta().y() < 0)
        mZoom = 1.1f * mZoom;
    else
        mZoom = mZoom / 1.1f;

    mZoom = qMax(1.0f, qMin(100.0f, mZoom));
}

void OrthographicCamera::update(float ifps)
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

QMatrix4x4 OrthographicCamera::getProjectionMatrix()
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
