#include "OrthographicCamera.h"

void LineOfSightAnalyzer::OrthographicCamera::Update(float Ifps)
{
    if (mUpdatePosition)
    {
        const auto &Movement = mMouse.GetCumulativeMovement(Qt::LeftButton);
        mLeft += Movement.x() * mZoom;
        mTop -= Movement.y() * mZoom;
        mMouse.ResetCumulativeMovement(Qt::LeftButton);
        mUpdatePosition = false;
    }
}

void LineOfSightAnalyzer::OrthographicCamera::Reset()
{
    mMouse.Reset();
    mUpdatePosition = false;
    mZoom = 2.0f;
    mLeft = 0.0f;
    mTop = 0.0f;
}

bool LineOfSightAnalyzer::OrthographicCamera::OnMousePressed(QMouseEvent *pEvent)
{
    bool Consumed = false;

    if (pEvent->button() == Qt::LeftButton)
    {
        mMouse.SetButtonPressed(Qt::LeftButton, true);
        mMouse.SetLastPressPosition(Qt::LeftButton, pEvent->position());
        Consumed = true;
    }

    return Consumed;
}

bool LineOfSightAnalyzer::OrthographicCamera::OnMouseReleased(QMouseEvent *pEvent)
{
    mMouse.SetButtonPressed(pEvent->button(), false);
    return false;
}

bool LineOfSightAnalyzer::OrthographicCamera::OnMouseMoved(QMouseEvent *pEvent)
{
    bool Consumed = false;

    if (mMouse.IsButtonPressed(Qt::LeftButton))
    {
        const auto Movement = pEvent->position() - mMouse.GetLastPressPosition(Qt::LeftButton);
        mMouse.AddCumulativeMovement(Qt::LeftButton, Movement);
        mMouse.SetLastPressPosition(Qt::LeftButton, pEvent->position());
        mUpdatePosition = true;
        Consumed = true;
    }

    return Consumed;
}

bool LineOfSightAnalyzer::OrthographicCamera::OnWheelMoved(QWheelEvent *pEvent)
{
    if (pEvent->angleDelta().y() < 0)
    {
        mZoom += mZoom / 16;
    }
    else
    {
        mZoom -= mZoom / 16;
    }

    mZoom = qMax(0.125f, qMin(8.0f, mZoom));
    return true; // Consume the event to prevent further propagation
}

bool LineOfSightAnalyzer::OrthographicCamera::OnLeaveEvent(QEvent *pEvent)
{
    mMouse.Reset();
    return false;
}

bool LineOfSightAnalyzer::OrthographicCamera::OnFocusOutEvent(QFocusEvent *pEvent)
{
    mMouse.Reset();
    return false;
}

QMatrix4x4 LineOfSightAnalyzer::OrthographicCamera::GetProjectionMatrix() const
{
    QMatrix4x4 Projection;
    Projection.ortho(-mZoom * mWidth / 2 - mLeft, //
                     mZoom * mWidth / 2 - mLeft,
                     -mZoom * mHeight / 2 - mTop,
                     mZoom * mHeight / 2 - mTop,
                     mZNear,
                     mZFar);
    return Projection;
}
