#include "Shader.h"

#include "Util/Util.h"

#include <QDebug>
#include <QFile>

LineOfSightAnalyzer::Shader::Shader(const QString& name)
    : mProgram(nullptr)
    , mName(name)
{
}

void LineOfSightAnalyzer::Shader::Initialize()
{
    LOG_INFO("Shader::Initialize: '{}' is being initializing.", mName.toStdString());

    initializeOpenGLFunctions();

    mProgram = QSharedPointer<QOpenGLShaderProgram>(new QOpenGLShaderProgram);

    for (const auto [shaderType, path] : mPaths)
    {
        const auto bytes = Util::GetBytes(path);
        if (!mProgram->addShaderFromSourceCode(shaderType, bytes))
        {
            LOG_FATAL("Shader::Initialize: '{}' could not be loaded.", GetShaderTypeString(shaderType).toStdString());
            FailureBehaviour::Failure(FailureType::COULD_NOT_LOAD_SHADER);
        }
    }

    if (!mProgram->link())
    {
        LOG_FATAL("Shader::Initialize: Could not link shader program.");
        FailureBehaviour::Failure(FailureType::COULD_NOT_LINK_SHADER);
    }

    if (!mProgram->bind())
    {
        LOG_FATAL("Shader::Initialize: Could not bind shader program.");
        FailureBehaviour::Failure(FailureType::COULD_NOT_BIND_SHADER);
    }

    LOG_INFO("Shader::Initialize: '{}' has been initialized.", mName.toStdString());
}

bool LineOfSightAnalyzer::Shader::Bind()
{
    return mProgram->bind();
}

void LineOfSightAnalyzer::Shader::Release()
{
    mProgram->release();
}

void LineOfSightAnalyzer::Shader::AddPath(QOpenGLShader::ShaderTypeBit type, const QString& path)
{
    mPaths.emplace(type, path);
}

QString LineOfSightAnalyzer::Shader::GetName() const
{
    return mName;
}

QString LineOfSightAnalyzer::Shader::GetShaderTypeString(QOpenGLShader::ShaderTypeBit type)
{
    switch (type)
    {
        case QOpenGLShader::Vertex:
            return "Vertex Shader";
        case QOpenGLShader::Fragment:
            return "Fragment Shader";
        case QOpenGLShader::Geometry:
            return "Geometry Shader";
        case QOpenGLShader::TessellationControl:
            return "Tessellation Control Shader";
        case QOpenGLShader::TessellationEvaluation:
            return "Tesselation Evaluation Shader";
        case QOpenGLShader::Compute:
            return "Compute Shader";
        default:
            return "Unknown Shader";
    }
}

void LineOfSightAnalyzer::Shader::ActivateTexture(const Texture& texture)
{
    SetUniformValue(texture.name, texture.unit);
    glActiveTexture(GL_TEXTURE0 + texture.unit);
    glBindTexture(texture.target, texture.id);
}