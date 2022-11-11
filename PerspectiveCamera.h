#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "Camera.h"

class PerspectiveCamera : public Camera
{
protected:
    explicit PerspectiveCamera(QObject *parent = nullptr);

public:
    void setVerticalFov(float newVerticalFov);
    void setHorizontalFov(float newHorizontalFov);
    float verticalFov() const;
    float horizontalFov() const;

    virtual QMatrix4x4 getProjectionMatrix() override;

protected:
    float mVerticalFov;
    float mHorizontalFov;
};

#endif // PERSPECTIVECAMERA_H
