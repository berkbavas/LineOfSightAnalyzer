#pragma once

#include "Node/Node.h"

#include <QOpenGLExtraFunctions>

namespace LineOfSightAnalyzer
{
    class Renderable : public Node, protected QOpenGLExtraFunctions
    {
        virtual void Construct() = 0;
        virtual void Render() = 0;
        virtual void Destroy() = 0;

        DEFINE_MEMBER(float, Ambient, 0.5f);
        DEFINE_MEMBER(float, Diffuse, 0.5f);
        DEFINE_MEMBER(float, Specular, 0.25f);
        DEFINE_MEMBER(QVector4D, Color, 1, 1, 1, 1);
    };
}