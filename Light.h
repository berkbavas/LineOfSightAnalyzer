#ifndef LIGHT_H
#define LIGHT_H

#include "Common.h"

#include <QObject>
#include <QVector3D>

class Light
{
public:
    Light();

protected:
    DEFINE_MEMBER(QVector3D, Direction)
    DEFINE_MEMBER(QVector4D, Color)
    DEFINE_MEMBER(float, Ambient)
    DEFINE_MEMBER(float, Diffuse)
    DEFINE_MEMBER(float, Specular)
};

#endif // LIGHT_H
