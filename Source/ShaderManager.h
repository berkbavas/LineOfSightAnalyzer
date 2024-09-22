#pragma once

#include "Common.h"

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

namespace LineOfSightAnalyzer
{
    class Shader;

    class ShaderManager : public QObject
    {
    private:
        explicit ShaderManager(QObject* parent = nullptr);

    public:
        Shader* GetShader(ShaderType shader);

        static ShaderManager* Instance();
        bool Init();

        bool Bind(ShaderType shader);
        void Release();

        void SetUniformValue(const QString& name, int value);
        void SetUniformValue(const QString& name, unsigned int value);
        void SetUniformValue(const QString& name, float value);
        void SetUniformValue(const QString& name, const QVector3D& value);
        void SetUniformValue(const QString& name, const QVector4D& value);
        void SetUniformValue(const QString& name, const QMatrix4x4& value);
        void SetUniformValue(const QString& name, const QMatrix3x3& value);
        void SetUniformValueArray(const QString& name, const QVector<QVector3D>& values);
        void SetSampler(const QString& name, unsigned int unit, unsigned int id, GLenum target = GL_TEXTURE_2D);

    private:
        ShaderType mActiveShader;
        QMap<ShaderType, Shader*> mShaders;
    };
}
