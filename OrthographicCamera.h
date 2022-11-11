#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H

#include "Camera.h"
#include "Mouse.h"

#include <QObject>

class OrthographicCamera : public Camera
{
    Q_OBJECT
public:
    explicit OrthographicCamera(QObject *parent = nullptr);

    virtual void keyPressed(QKeyEvent *) override;
    virtual void keyReleased(QKeyEvent *) override;
    virtual void mousePressed(QMouseEvent *event) override;
    virtual void mouseReleased(QMouseEvent *event) override;
    virtual void mouseMoved(QMouseEvent *event) override;
    virtual void wheelMoved(QWheelEvent *event) override;
    virtual void update(float) override;

    virtual QMatrix4x4 getProjectionMatrix() override;

private:
    DEFINE_MEMBER(float, Zoom)
    DEFINE_MEMBER(float, Left)
    DEFINE_MEMBER(float, Top)

    Mouse mMouse;
    bool mUpdatePosition;

    float mTimeElapsed;
};

#endif // ORTHOGRAPHICCAMERA_H
