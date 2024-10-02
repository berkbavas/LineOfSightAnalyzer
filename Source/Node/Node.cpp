#include "Node.h"

void LineOfSightAnalyzer::Node::SetRotation(const QQuaternion& newRotation)
{
    if (mRotation == newRotation)
    {
        return;
    }

    mRotation = newRotation;
    Node::UpdateTransformation();
}

void LineOfSightAnalyzer::Node::SetPosition(const QVector3D& newPosition)
{
    if (mPosition == newPosition)
    {
        return;
    }

    mPosition = newPosition;
    Node::UpdateTransformation();
}

void LineOfSightAnalyzer::Node::SetScale(const QVector3D& newScale)
{
    if (mScale == newScale)
    {
        return;
    }

    mScale = newScale;
    Node::UpdateTransformation();
}

void LineOfSightAnalyzer::Node::SetTransformation(const QMatrix4x4& newTransformation)
{
    if (mTransformation == newTransformation)
    {
        return;
    }

    mTransformation = newTransformation;
    mPosition = mTransformation.column(3).toVector3D();
    mRotation = QQuaternion::fromRotationMatrix(mTransformation.normalMatrix());
}

void LineOfSightAnalyzer::Node::SetPosition(float x, float y, float z)
{
    SetPosition(QVector3D(x, y, z));
}

void LineOfSightAnalyzer::Node::SetScale(float x, float y, float z)
{
    SetScale(QVector3D(x, y, z));
}

void LineOfSightAnalyzer::Node::UpdateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));
}

void LineOfSightAnalyzer::Node::RotateGlobal(const QVector3D& axis, float angle)
{
    SetRotation(QQuaternion::fromAxisAndAngle(axis, angle) * GetRotation());
}

void LineOfSightAnalyzer::Node::RotateLocal(const QVector3D& axis, float angle)
{
    SetRotation(GetRotation() * QQuaternion::fromAxisAndAngle(axis, angle));
}

void LineOfSightAnalyzer::Node::Translate(const QVector3D& delta)
{
    SetPosition(mPosition + delta);
}
