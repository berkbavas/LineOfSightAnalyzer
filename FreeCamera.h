#ifndef FREECAMERA_H
#define FREECAMERA_H

#include "Mouse.h"
#include "PerspectiveCamera.h"

#include <QObject>

class FreeCamera : public PerspectiveCamera
{
    Q_OBJECT

public:
    explicit FreeCamera(QObject *parent = nullptr);
    virtual ~FreeCamera();

    void keyPressed(QKeyEvent *event) override;
    void keyReleased(QKeyEvent *event) override;
    void mousePressed(QMouseEvent *event) override;
    void mouseReleased(QMouseEvent *event) override;
    void mouseMoved(QMouseEvent *event) override;
    void wheelMoved(QWheelEvent *event) override;
    void update(float ifps) override;

    QVector3D getEulerDegrees() const;

private:
    QMap<Qt::Key, bool> mPressedKeys;

    Mouse mMouse;

    bool mUpdateRotation;
    bool mUpdatePosition;

    static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
};

#endif // FREECAMERA_H
