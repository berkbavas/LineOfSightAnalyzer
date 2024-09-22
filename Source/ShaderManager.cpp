#include "ShaderManager.h"
#include "Shader.h"

LineOfSightAnalyzer::ShaderManager::ShaderManager(QObject* parent)
    : QObject(parent)
    , mActiveShader(ShaderType::None)
{}

LineOfSightAnalyzer::Shader* LineOfSightAnalyzer::ShaderManager::GetShader(ShaderType shader)
{
    return mShaders.value(shader);
}

bool LineOfSightAnalyzer::ShaderManager::Init()
{
    // Terrain Shader
    {
        Shader* shader = new Shader(ShaderType::TerrainShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Terrain.frag");

        if (!shader->Init())
            return false;
    }

    // Observer Shader
    {
        Shader* shader = new Shader(ShaderType::ObserverShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Observer.vert");
        shader->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/Observer.geom");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Observer.frag");

        if (!shader->Init())
            return false;
    }

    // Debug Shader
    {
        Shader* shader = new Shader(ShaderType::DebugShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Debug.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Debug.frag");

        if (!shader->Init())
            return false;
    }

    return true;
}

bool LineOfSightAnalyzer::ShaderManager::Bind(ShaderType shader)
{
    mActiveShader = shader;
    return mShaders.value(mActiveShader)->Bind();
}

void LineOfSightAnalyzer::ShaderManager::Release()
{
    mShaders.value(mActiveShader)->Release();
}

void LineOfSightAnalyzer::ShaderManager::SetUniformValue(const QString& name, int value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void LineOfSightAnalyzer::ShaderManager::SetUniformValue(const QString& name, unsigned int value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void LineOfSightAnalyzer::ShaderManager::SetUniformValue(const QString& name, float value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void LineOfSightAnalyzer::ShaderManager::SetUniformValue(const QString& name, const QVector3D& value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void LineOfSightAnalyzer::ShaderManager::SetUniformValue(const QString& name, const QVector4D& value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void LineOfSightAnalyzer::ShaderManager::SetUniformValue(const QString& name, const QMatrix4x4& value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void LineOfSightAnalyzer::ShaderManager::SetUniformValue(const QString& name, const QMatrix3x3& value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void LineOfSightAnalyzer::ShaderManager::SetUniformValueArray(const QString& name, const QVector<QVector3D>& values)
{
    mShaders.value(mActiveShader)->SetUniformValueArray(name, values);
}

void LineOfSightAnalyzer::ShaderManager::SetSampler(const QString& name, unsigned int unit, unsigned int id, GLenum target)
{
    mShaders.value(mActiveShader)->SetSampler(name, unit, id, target);
}

LineOfSightAnalyzer::ShaderManager* LineOfSightAnalyzer::ShaderManager::Instance()
{
    static ShaderManager instance;

    return &instance;
}