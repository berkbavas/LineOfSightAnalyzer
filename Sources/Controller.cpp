#include "Controller.h"
#include "Constants.h"
#include "Window.h"

#include <QApplication>
#include <QDebug>

LineOfSightAnalyzer::Controller::Controller(QObject *parent)
    : QObject(parent)
    , mWidth(DEFUALT_SCREEN_WIDTH)
    , mHeight(DEFUALT_SCREEN_HEIGHT)
    , mDevicePixelRatio(1.0f)
    , mActiveCamera(nullptr)
    , mMinElevation(0)
    , mMaxElevation(1000)
    , mMaxDistance(1000.0f)
    , mObserverHeight(45.0f)
    , mLockObserver(false)
    , mDebugEnabled(false)
    , mBias(0.005f)
    , mColorAttachments{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1}
{
    mCamera3D = new FreeCamera;
    mCamera3D->SetPosition(QVector3D(0, 1000, 0));
    mCamera3D->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
    mCamera3D->SetVerticalFov(50.0f);
    mCamera3D->SetZNear(1.0f);
    mCamera3D->SetZFar(1000000.0f);
    mCamera3D->Resize(mWidth, mHeight);

    mCamera2D = new OrthographicCamera;
    mCamera2D->SetPosition(QVector3D(0, 100000, 0));
    mCamera2D->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
    mCamera2D->SetZNear(1.0f);
    mCamera2D->SetZFar(1000000.0f);
    mCamera2D->Resize(mWidth, mHeight);
    SetActiveCamera(mCamera2D);

    mFBOFormats.insert(FramebufferType::Terrain, new QOpenGLFramebufferObjectFormat);
    mFBOFormats[FramebufferType::Terrain]->setAttachment(QOpenGLFramebufferObject::Depth);
    mFBOFormats[FramebufferType::Terrain]->setSamples(0);
    mFBOFormats[FramebufferType::Terrain]->setTextureTarget(GL_TEXTURE_2D);
}

LineOfSightAnalyzer::Controller::~Controller() {}

void LineOfSightAnalyzer::Controller::Init()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    QString path = ":/Resources/HeightMaps/00.png";
    QImage image(path);

    if (image.isNull())
        qWarning() << "An image at " + path + " is null.";

    mHeightMap = new QOpenGLTexture(image, QOpenGLTexture::GenerateMipMaps);
    mHeightMap->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);
    mHeightMap->setMinMagFilters(QOpenGLTexture::Filter::LinearMipMapLinear, QOpenGLTexture::Filter::Linear);

    mTerrain = new Terrain(mHeightMap->width(), mHeightMap->height());

    mQuad = new Quad;

    mShaderManager = ShaderManager::Instance();
    mShaderManager->Init();

    // Line of sight stuff
    {
        for (int i = 0; i < 6; i++)
        {
            mObservers << new FreeCamera;
            mObservers[i]->SetPosition(QVector3D(0, 100, 0));
            mObservers[i]->Resize(OBSERVER_FBO_WIDTH, OBSERVER_FBO_HEIGHT);
            mObservers[i]->SetVerticalFov(90.0f);
            mObservers[i]->SetZNear(10.0f);
            mObservers[i]->SetZFar(1000.0f);
        }
        auto rollFix = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), 180);

        mObservers[0]->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), -90) * rollFix);
        mObservers[1]->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 90) * rollFix);
        mObservers[2]->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90));
        mObservers[3]->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
        mObservers[4]->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180) * rollFix);
        mObservers[5]->SetRotation(rollFix);

        glGenFramebuffers(1, &mObserverFBO);
        glGenTextures(1, &mObserverFBODepthMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mObserverFBODepthMap);
        for (unsigned int i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, OBSERVER_FBO_WIDTH, OBSERVER_FBO_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, mObserverFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mObserverFBODepthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    CreateFramebuffers();
}

void LineOfSightAnalyzer::Controller::Render(float ifps)
{
    mDevicePixelRatio = mWindow->devicePixelRatioF();

    mIfps = ifps;
    mActiveCamera->Update(mIfps);
    mActiveCamera->SetDevicePixelRatio(mDevicePixelRatio);

    // Update observer position
    for (int i = 0; i < 6; i++)
    {
        if (!mLockObserver)
            mObservers[i]->SetPosition(mMouseWorldPosition + QVector3D(0, mObserverHeight, 0));

        if (mDebugEnabled)
            mObservers[i]->SetPosition(mCamera3D->Position());

        mObservers[i]->SetZFar(mMaxDistance);
    }

    // Observer
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mObserverFBO);
        glViewport(0, 0, OBSERVER_FBO_WIDTH, OBSERVER_FBO_HEIGHT);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mShaderManager->Bind(ShaderType::ObserverShader);

        for (int i = 0; i < 6; i++)
        {
            mShaderManager->SetUniformValue(QString("VPs[%1]").arg(i), mObservers[i]->GetViewProjectionMatrix());
        }

        mShaderManager->SetUniformValue("minElevation", mMinElevation);
        mShaderManager->SetUniformValue("maxElevation", mMaxElevation);
        mShaderManager->SetSampler("heightMap", 0, mHeightMap->textureId());
        mShaderManager->SetUniformValue("observerPosition", mObservers[0]->Position());
        mShaderManager->SetUniformValue("farPlane", mObservers[0]->GetZFar());
        mTerrain->Render();
        mShaderManager->Release();
    }

    // DEBUG
    if (mDebugEnabled)
    {
        QOpenGLFramebufferObject::bindDefault();
        glViewport(0, 0, mWidth, mHeight);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mShaderManager->Bind(ShaderType::DebugShader);
        mShaderManager->SetUniformValue("IVP", mCamera3D->GetRotationMatrix().inverted() * mCamera3D->GetProjectionMatrix().inverted());
        mShaderManager->SetSampler("depthMap", 1, mObserverFBODepthMap, GL_TEXTURE_CUBE_MAP);
        mQuad->render();
        mShaderManager->Release();
    }

    // Terrain
    if (!mDebugEnabled)
    {
        mFBOs[FramebufferType::Terrain]->bind();
        glViewport(0, 0, mWidth, mHeight);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mShaderManager->Bind(ShaderType::TerrainShader);
        mShaderManager->SetUniformValue("VP", mActiveCamera->GetViewProjectionMatrix());
        mShaderManager->SetUniformValue("minElevation", mMinElevation);
        mShaderManager->SetUniformValue("maxElevation", mMaxElevation);
        mShaderManager->SetUniformValue("maxDistance", mMaxDistance);
        mShaderManager->SetSampler("heightMap", 0, mHeightMap->textureId());
        mShaderManager->SetSampler("depthMap", 1, mObserverFBODepthMap, GL_TEXTURE_CUBE_MAP);
        mShaderManager->SetUniformValue("observerPosition", mObservers[0]->Position());
        mShaderManager->SetUniformValue("farPlane", mObservers[0]->GetZFar());
        mShaderManager->SetUniformValue("maxDistance", mMaxDistance);
        mShaderManager->SetUniformValue("bias", mBias);
        mTerrain->Render();
        mShaderManager->Release();

        QOpenGLFramebufferObject::bindDefault();
        glViewport(0, 0, mWidth, mHeight);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        QOpenGLFramebufferObject::blitFramebuffer(nullptr, //
                                                  QRect(0, 0, mWidth, mHeight),
                                                  mFBOs[FramebufferType::Terrain],
                                                  QRect(0, 0, mFBOs[FramebufferType::Terrain]->width(), mFBOs[FramebufferType::Terrain]->height()),
                                                  GL_COLOR_BUFFER_BIT,
                                                  GL_NEAREST,
                                                  0,
                                                  0);
    }

    // ImGui Stuff
    QtImGui::newFrame();
    //ImGui::ShowDemoWindow();
    DrawGUI();
    ImGui::Render();
    QtImGui::render();
}

void LineOfSightAnalyzer::Controller::DrawGUI()
{
    auto viewDir = mActiveCamera->GetViewDirection();

    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls");

    // Render Settings
    if (!ImGui::CollapsingHeader("Render Settings"))
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Observer");
        ImGui::SliderFloat("Max Distance##ObserverSettings", &mMaxDistance, 1, 5000);
        ImGui::SliderFloat("Height##ObserverSettings", &mObserverHeight, 0, 1000);

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Elevation");
        ImGui::SliderFloat("Min Elevation##RenderSettings", &mMinElevation, 1, 1000);
        ImGui::SliderFloat("Max Elevation##RenderSettings", &mMaxElevation, mMinElevation, 10000);

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Debug");
        ImGui::SliderFloat("Bias##RenderSettings", &mBias, 0, 0.1);
        ImGui::Checkbox("Debug", &mDebugEnabled);
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Info");
    ImGui::Text("Observer Position: (%.1f, %.1f,%.1f)", mObservers[0]->Position().x(), mObservers[0]->Position().y(), mObservers[0]->Position().z());
    ImGui::Text("Mouse World Position: (%.1f, %.1f, %.1f)", mMouseWorldPosition[0], mMouseWorldPosition[1], mMouseWorldPosition[2]);
    ImGui::Text("Camera View Direction: (%.1f, %.1f, %.1f)", viewDir.x(), viewDir.y(), viewDir.z());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Hints");
    ImGui::Text("Observer Lock / Unlock: L");
    ImGui::Text("Go to Observer's Position: G");
    ImGui::Text("Camera Speed Fast: Shift");
    ImGui::Text("Camera Speed Very Fast: Ctrl");

    ImGui::End();
}

void LineOfSightAnalyzer::Controller::SetActiveCamera(Camera *newActiveCamera)
{
    if (newActiveCamera == mActiveCamera)
        return;

    if (newActiveCamera)
        newActiveCamera->SetActive(true);

    if (mActiveCamera)
        mActiveCamera->SetActive(false);

    mActiveCamera = newActiveCamera;
}

void LineOfSightAnalyzer::Controller::SetWindow(Window *newWindow)
{
    mWindow = newWindow;
}

void LineOfSightAnalyzer::Controller::WheelMoved(QWheelEvent *event)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mActiveCamera->WheelMoved(event);
}

void LineOfSightAnalyzer::Controller::MousePressed(QMouseEvent *event)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mActiveCamera->MousePressed(event);
}

void LineOfSightAnalyzer::Controller::MouseReleased(QMouseEvent *event)
{
    mActiveCamera->MouseReleased(event);
}

void LineOfSightAnalyzer::Controller::MouseMoved(QMouseEvent *event)
{
    mMouseMoving = true;
    QTimer::singleShot(100, [=]() { mMouseMoving = false; });

    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mFBOs[FramebufferType::Terrain]->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(event->pos().x() * mDevicePixelRatio, mFBOs[FramebufferType::Terrain]->height() - event->pos().y() * mDevicePixelRatio, 1, 1, GL_RGB, GL_FLOAT, &mMouseWorldPosition);
    mFBOs[FramebufferType::Terrain]->release();

    mActiveCamera->MouseMoved(event);
}

void LineOfSightAnalyzer::Controller::KeyPressed(QKeyEvent *event)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
        return;

    if (event->key() == Qt::Key_1)
        SetActiveCamera(mCamera3D);
    else if (event->key() == Qt::Key_2)
        SetActiveCamera(mCamera2D);
    else if (event->key() == Qt::Key_L)
        mLockObserver = !mLockObserver;
    else if (event->key() == Qt::Key_G)
    {
        mCamera3D->SetPosition(mObservers[0]->Position());
        mCamera3D->Resize(mWidth, mHeight);
        SetActiveCamera(mCamera3D);
    } else
        mActiveCamera->KeyPressed(event);
}

void LineOfSightAnalyzer::Controller::KeyReleased(QKeyEvent *event)
{
    mActiveCamera->KeyReleased(event);
}

void LineOfSightAnalyzer::Controller::Resize(int width, int height)
{
    mWindow->makeCurrent();

    mWidth = width * mDevicePixelRatio;
    mHeight = height * mDevicePixelRatio;

    DeleteFramebuffers();
    CreateFramebuffers();

    mActiveCamera->Resize(mWidth, mHeight);

    mWindow->doneCurrent();
}

void LineOfSightAnalyzer::Controller::DeleteFramebuffers()
{
    auto keys = mFBOs.keys();

    for (auto type : keys)
        if (mFBOs[type])
            delete mFBOs[type];
}

void LineOfSightAnalyzer::Controller::CreateFramebuffers()
{
    mFBOs.insert(FramebufferType::Terrain, new QOpenGLFramebufferObject(mWidth, mHeight, *mFBOFormats[FramebufferType::Terrain]));
    mFBOs[FramebufferType::Terrain]->addColorAttachment(mWidth, mHeight, GL_RGB32F);

    mFBOs[FramebufferType::Terrain]->bind();
    glDrawBuffers(2, mColorAttachments);
    mFBOs[FramebufferType::Terrain]->release();
}
