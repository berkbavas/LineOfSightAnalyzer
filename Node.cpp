#include "Node.h"

#include <QtMath>

Node::Node(QObject *parent)
    : QObject{parent}
    , mPosition(0, 0, 0)
    , mScale(1, 1, 1)

{}

Node::~Node() {}

const QMatrix4x4 &Node::transformation() const
{
    return mTransformation;
}

void Node::setTransformation(const QMatrix4x4 &newTransformation)
{
    mTransformation = newTransformation;
    mPosition = mTransformation.column(3).toVector3D();
    mRotation = QQuaternion::fromRotationMatrix(mTransformation.normalMatrix());
}

const QQuaternion &Node::rotation() const
{
    return mRotation;
}

void Node::setRotation(const QQuaternion &newRotation)
{
    mRotation = newRotation;
    updateTransformation();
}

const QVector3D &Node::position() const
{
    return mPosition;
}

void Node::setPosition(const QVector3D &newPosition)
{
    mPosition = newPosition;
    updateTransformation();
}

const QVector3D &Node::scale() const
{
    return mScale;
}

void Node::setScale(const QVector3D &newScale)
{
    mScale = newScale;
    updateTransformation();
}

void Node::updateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));
}
