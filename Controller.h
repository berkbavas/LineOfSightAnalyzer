#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "FreeCamera.h"
#include "OrthographicCamera.h"
#include "Quad.h"
#include "ShaderManager.h"
#include "Terrain.h"

#include <imgui.h>
#include <QtImGui.h>

#include <QObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLTexture>

class Window;

class Controller : public QObject, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    virtual ~Controller();

    void init();
    void wheelMoved(QWheelEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
    void keyPressed(QKeyEvent *event);
    void keyReleased(QKeyEvent *event);
    void resize(int width, int height);
    void render(float ifps);
    void drawGUI();
    void setWindow(Window *newWindow);

private slots:
    void setActiveCamera(Camera *newActiveCamera);
    void deleteFramebuffers();
    void createFramebuffers();

private:
    enum class FramebufferType { //
        Terrain,
    };

    Window *mWindow;
    int mWidth;
    int mHeight;
    float mDevicePixelRatio;

    FreeCamera *mCamera3D;
    OrthographicCamera *mCamera2D;
    Camera *mActiveCamera;

    ShaderManager *mShaderManager;
    Terrain *mTerrain;
    QOpenGLTexture *mHeightMap;
    Quad *mQuad;

    float mIfps;

    // Terrain
    float mMinElevation;
    float mMaxElevation;

    // LOS Stuff
    QVector<FreeCamera *> mObservers;
    float mMaxDistance;
    float mObserverHeight;
    bool mLockObserver;
    bool mMouseMoving;
    QVector3D mMouseWorldPosition;

    QMap<FramebufferType, QOpenGLFramebufferObjectFormat *> mFBOFormats;
    QMap<FramebufferType, QOpenGLFramebufferObject *> mFBOs;
    GLuint mColorAttachments[2];
};

#endif // CONTROLLER_H
