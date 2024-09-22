#include "Node.h"

#include <QtMath>

LineOfSightAnalyzer::Node::Node(QObject* parent)
    : QObject(parent)
    , mPosition(0, 0, 0)
    , mScale(1, 1, 1)

{}

LineOfSightAnalyzer::Node::~Node() {}

const QMatrix4x4& LineOfSightAnalyzer::Node::Transformation() const
{
    return mTransformation;
}

void LineOfSightAnalyzer::Node::SetTransformation(const QMatrix4x4& newTransformation)
{
    mTransformation = newTransformation;
    mPosition = mTransformation.column(3).toVector3D();
    mRotation = QQuaternion::fromRotationMatrix(mTransformation.normalMatrix());
}

const QQuaternion& LineOfSightAnalyzer::Node::Rotation() const
{
    return mRotation;
}

void LineOfSightAnalyzer::Node::SetRotation(const QQuaternion& newRotation)
{
    mRotation = newRotation;
    UpdateTransformation();
}

const QVector3D& LineOfSightAnalyzer::Node::Position() const
{
    return mPosition;
}

void LineOfSightAnalyzer::Node::SetPosition(const QVector3D& newPosition)
{
    mPosition = newPosition;
    UpdateTransformation();
}

const QVector3D& LineOfSightAnalyzer::Node::Scale() const
{
    return mScale;
}

void LineOfSightAnalyzer::Node::SetScale(const QVector3D& newScale)
{
    mScale = newScale;
    UpdateTransformation();
}

void LineOfSightAnalyzer::Node::UpdateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));
}