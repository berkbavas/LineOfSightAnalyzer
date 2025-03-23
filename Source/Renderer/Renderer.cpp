#include "Renderer.h"

#include "Core/CameraManager.h"
#include "Core/Constants.h"
#include "Renderer/LineOfSightRenderer.h"
#include "Renderer/TerrainRenderer.h"
#include "Util/Logger.h"
#include "Util/Math.h"
#include "Util/Util.h"

#include <QOpenGLFramebufferObject>
#include <QtImGui.h>
#include <imgui.h>

void LineOfSightAnalyzer::Renderer::Initialize()
{
    initializeOpenGLFunctions();

    glDisable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    mTerrainRenderer = new TerrainRenderer;
    mLineOfSightRenderer = new LineOfSightRenderer;

    mTerrainRenderer->SetLineOfSightRenderer(mLineOfSightRenderer);
    mTerrainRenderer->SetCameraManager(mCameraManager);
    mTerrainRenderer->Init();

    mLineOfSightRenderer->SetTerrainRenderer(mTerrainRenderer);
    mLineOfSightRenderer->Init();
}

void LineOfSightAnalyzer::Renderer::Render(float ifps)
{
    mLineOfSightRenderer->Render(ifps);

    mTerrainRenderer->SetDevicePixelRatio(mDevicePixelRatio);
    mTerrainRenderer->Render(ifps);
}

void LineOfSightAnalyzer::Renderer::DrawGui()
{
    mLineOfSightRenderer->DrawGui();
    mTerrainRenderer->DrawGui();
}

void LineOfSightAnalyzer::Renderer::Resize(int width, int height)
{
    mTerrainRenderer->SetDevicePixelRatio(mDevicePixelRatio);
    mTerrainRenderer->Resize(width, height);
}

void LineOfSightAnalyzer::Renderer::OnMouseMoved(QMouseEvent* event)
{
    mTerrainRenderer->onMouseMoved(event);
}

void LineOfSightAnalyzer::Renderer::OnKeyPressed(QKeyEvent* event)
{
    if(event->key() == Qt::Key_L)
    {
        mLineOfSightRenderer->SetLockObserverPosition(!mLineOfSightRenderer->GetLockObserverPosition());
    }
}
