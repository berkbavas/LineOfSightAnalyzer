#pragma once

#include "Core/Constants.h"
#include "Core/OpenGLWidget.h"
#include "Manager/CameraManager.h"
#include "Manager/LineOfSightRenderer.h"
#include "Manager/TerrainRenderer.h"
#include "Util/Macros.h"

#include <memory>

#include <QMouseEvent>
#include <QOpenGLExtraFunctions>
#include <QtImGui.h>

namespace LineOfSightAnalyzer
{
    class Controller : public QObject, protected QOpenGLExtraFunctions
    {
        Q_OBJECT
      public:
        explicit Controller(QObject* pParent = nullptr);

        void Run();

      private:
        void Initialize();
        void Resize(int Width, int Height);
        void Render(float Ifps);
        void OnKeyPressed(QKeyEvent* pEvent);
        void OnKeyReleased(QKeyEvent* pEvent);
        void OnMousePressed(QMouseEvent* pEvent);
        void OnMouseReleased(QMouseEvent* pEvent);
        void OnMouseMoved(QMouseEvent* pEvent);
        void OnWheelMoved(QWheelEvent* pEvent);
        void OnLeaveEvent(QEvent* pEvent);
        void OnFocusOutEvent(QFocusEvent* pEvent);
        void DrawStats();

        OpenGLWidgetPtr mWindow;
        CameraManagerPtr mCameraManager;
        LineOfSightRendererPtr mLineOfSightRenderer;
        TerrainRendererPtr mTerrainRenderer;

        QtImGui::RenderRef mRenderRef{ nullptr };
        QVector<EventReceiver*> mEventReceivers;
    };
}
