#ifndef CAMERA_H
#define CAMERA_H

#include "Common.h"
#include "Node.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QTimer>

class Camera : public Node
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = nullptr);

    virtual void keyPressed(QKeyEvent *) = 0;
    virtual void keyReleased(QKeyEvent *) = 0;
    virtual void mousePressed(QMouseEvent *event) = 0;
    virtual void mouseReleased(QMouseEvent *event) = 0;
    virtual void mouseMoved(QMouseEvent *event) = 0;
    virtual void wheelMoved(QWheelEvent *event) = 0;
    virtual void resize(int width, int height);
    virtual void update(float) = 0;

    virtual QMatrix4x4 getProjectionMatrix() = 0;
    virtual QMatrix4x4 getViewMatrix();
    virtual QMatrix4x4 getViewProjectionMatrix();
    virtual QMatrix4x4 getRotationMatrix();

    bool active() const;
    void setActive(bool newActive);

signals:
    void activeChanged();

protected:
    bool mActive;

    DEFINE_MEMBER(int, Width)
    DEFINE_MEMBER(int, Height)
    DEFINE_MEMBER(float, DevicePixelRatio)
    DEFINE_MEMBER(float, ZNear) DEFINE_MEMBER(float, ZFar)
};

#endif // CAMERA_H
