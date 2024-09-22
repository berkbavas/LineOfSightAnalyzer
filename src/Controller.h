#pragma once

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

namespace LineOfSightAnalyzer
{
    class Window;

    class Controller : public QObject, protected QOpenGLFunctions_4_3_Core
    {
        Q_OBJECT
    public:
        explicit Controller(QObject* parent = nullptr);
        virtual ~Controller();

        void Init();
        void WheelMoved(QWheelEvent* event);
        void MousePressed(QMouseEvent* event);
        void MouseReleased(QMouseEvent* event);
        void MouseMoved(QMouseEvent* event);
        void KeyPressed(QKeyEvent* event);
        void KeyReleased(QKeyEvent* event);
        void Resize(int width, int height);
        void Render(float ifps);
        void DrawGUI();
        void SetWindow(Window* newWindow);

    private slots:
        void SetActiveCamera(Camera* newActiveCamera);
        void DeleteFramebuffers();
        void CreateFramebuffers();

    private:
        enum class FramebufferType { //
            Terrain,
        };

        Window* mWindow;
        int mWidth;
        int mHeight;
        float mDevicePixelRatio;

        FreeCamera* mCamera3D;
        OrthographicCamera* mCamera2D;
        Camera* mActiveCamera;

        ShaderManager* mShaderManager;
        Terrain* mTerrain;
        QOpenGLTexture* mHeightMap;
        Quad* mQuad;

        float mIfps;

        // Terrain
        float mMinElevation;
        float mMaxElevation;

        unsigned int mObserverFBO;
        unsigned int mObserverFBODepthMap;

        // LOS Stuff
        QVector<FreeCamera*> mObservers;
        float mMaxDistance;
        float mObserverHeight;
        bool mLockObserver;
        bool mMouseMoving;
        QVector3D mMouseWorldPosition;
        bool mDebugEnabled;
        float mBias;

        QMap<FramebufferType, QOpenGLFramebufferObjectFormat*> mFBOFormats;
        QMap<FramebufferType, QOpenGLFramebufferObject*> mFBOs;
        GLuint mColorAttachments[2];
    };
}
