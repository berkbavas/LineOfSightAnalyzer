#include "TerrainRenderer.h"

#include "Core/CameraManager.h"
#include "Renderer/LineOfSightRenderer.h"

#include <QtImgui.h>
#include <imgui.h>

void LineOfSightAnalyzer::TerrainRenderer::Init()
{
    initializeOpenGLFunctions();

    mFramebufferFormat = QOpenGLFramebufferObjectFormat();
    mFramebufferFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    mFramebufferFormat.setSamples(0);
    mFramebufferFormat.setTextureTarget(GL_TEXTURE_2D);

    mTerrainShader = new Shader("Terrain Shader");
    mTerrainShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
    mTerrainShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tcs");
    mTerrainShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tes");
    mTerrainShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Terrain.frag");
    mTerrainShader->Initialize();

    mTextureLoader = new TextureLoader;

    Texture texture = mTextureLoader->LoadTexture2D("Resources/HeightMaps/01.png");
    texture.name = "heightMap";
    texture.unit = 0;

    mTerrain = new Terrain(texture.width, texture.height, 16);
    mTerrain->SetTexture(texture);
    mTerrain->Construct();
}

void LineOfSightAnalyzer::TerrainRenderer::Render(float)
{
    mActiveCamera = mCameraManager->GetActiveCamera();

    mFramebuffer->bind();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable wireframe mode if selected
    if (mWireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    mTerrainShader->Bind();
    mTerrainShader->SetUniformValue("model", mTerrain->GetTransformation());
    mTerrainShader->SetUniformValue("view", mActiveCamera->GetViewMatrix());
    mTerrainShader->SetUniformValue("projection", mActiveCamera->GetProjectionMatrix());
    mTerrainShader->SetUniformValue("minElevation", mTerrain->GetMinElevation());
    mTerrainShader->SetUniformValue("maxElevation", mTerrain->GetMaxElevation());
    mTerrainShader->SetUniformValue("observerPosition", mLineOfSightAnalyzer->GetObservers().at(0)->GetPosition());
    mTerrainShader->SetUniformValue("farPlane", mLineOfSightAnalyzer->GetObservers().at(0)->GetZFar());
    mTerrainShader->SetUniformValue("bias", mBias);

    // Visualization uniforms
    mTerrainShader->SetUniformValue("colorScheme", static_cast<int>(mColorScheme));
    mTerrainShader->SetUniformValue("visibilityOpacity", mVisibilityOpacity);
    mTerrainShader->SetUniformValue("showLOS", mShowLOS ? 1 : 0);
    mTerrainShader->SetUniformValue("terrainColorMode", static_cast<int>(mTerrainColorMode));
    
    // Terrain analysis uniforms
    mTerrainShader->SetUniformValue("terrainAnalysisMode", static_cast<int>(mTerrainAnalysisMode));
    mTerrainShader->SetUniformValue("slopeThreshold", mSlopeThreshold);
    
    // Observer mode uniforms
    mTerrainShader->SetUniformValue("observerMode", static_cast<int>(mLineOfSightAnalyzer->GetObserverMode()));
    mTerrainShader->SetUniformValue("coneAngle", mLineOfSightAnalyzer->GetConeAngle());
    mTerrainShader->SetUniformValue("coneDirection", mLineOfSightAnalyzer->GetConeDirection());
    mTerrainShader->SetUniformValue("conePitch", mLineOfSightAnalyzer->GetConePitch());
    mTerrainShader->SetUniformValue("verticalAngleMin", mLineOfSightAnalyzer->GetVerticalAngleMin());
    mTerrainShader->SetUniformValue("verticalAngleMax", mLineOfSightAnalyzer->GetVerticalAngleMax());
    
    // Overlay uniforms
    mTerrainShader->SetUniformValue("showContourLines", mShowContourLines ? 1 : 0);
    mTerrainShader->SetUniformValue("contourInterval", mContourInterval);
    mTerrainShader->SetUniformValue("contourLineWidth", mContourLineWidth);
    mTerrainShader->SetUniformValue("showDistanceRings", mShowDistanceRings ? 1 : 0);
    mTerrainShader->SetUniformValue("distanceRingInterval", mDistanceRingInterval);
    mTerrainShader->SetUniformValue("showGrid", mShowGrid ? 1 : 0);
    mTerrainShader->SetUniformValue("gridSize", mGridSize);

    mTerrainShader->ActivateTexture(mTerrain->GetTexture());
    mTerrainShader->ActivateTexture(mLineOfSightAnalyzer->GetDepthMap());
    mTerrain->Render();
    mTerrainShader->Release();

    // Restore fill mode
    if (mWireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    QOpenGLFramebufferObject::bindDefault();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QOpenGLFramebufferObject::blitFramebuffer(nullptr, //
                                              QRect(0, 0, mWidth, mHeight),
                                              mFramebuffer,
                                              QRect(0, 0, mFramebuffer->width(), mFramebuffer->height()),
                                              GL_COLOR_BUFFER_BIT,
                                              GL_NEAREST,
                                              0,
                                              0);
}

void LineOfSightAnalyzer::TerrainRenderer::DrawGui()
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Elevation");
    ImGui::SliderFloat("Min Elevation##RenderSettings", &mTerrain->GetMinElevation_NonConst(), 0, 100);
    ImGui::SliderFloat("Max Elevation##RenderSettings", &mTerrain->GetMaxElevation_NonConst(), mTerrain->GetMinElevation(), 1000);
    ImGui::SliderFloat("Bias##RenderSettings", &mBias, 0.0f, 0.1f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Visualization");

    // LOS Toggle
    ImGui::Checkbox("Show Line of Sight", &mShowLOS);

    // Color Scheme dropdown
    const char* colorSchemes[] = { "Green-Red", "Blue-Yellow", "Heat Map", "Purple-Cyan" };
    int currentScheme = static_cast<int>(mColorScheme);
    if (ImGui::Combo("LOS Color Scheme", &currentScheme, colorSchemes, IM_ARRAYSIZE(colorSchemes)))
    {
        mColorScheme = static_cast<ColorScheme>(currentScheme);
    }

    // Visibility Opacity
    ImGui::SliderFloat("LOS Opacity", &mVisibilityOpacity, 0.0f, 1.0f);

    // Terrain Color Mode
    const char* terrainModes[] = { "Texture", "Height-Based", "Grayscale", "Heightmap" };
    int currentTerrainMode = static_cast<int>(mTerrainColorMode);
    if (ImGui::Combo("Terrain Color Mode", &currentTerrainMode, terrainModes, IM_ARRAYSIZE(terrainModes)))
    {
        mTerrainColorMode = static_cast<TerrainColorMode>(currentTerrainMode);
    }

    // Wireframe toggle
    ImGui::Checkbox("Wireframe Mode", &mWireframeMode);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Terrain Analysis");
    
    // Terrain Analysis Mode
    const char* analysisModes[] = { "None", "Slope", "Aspect (Direction)", "Curvature" };
    int currentAnalysisMode = static_cast<int>(mTerrainAnalysisMode);
    if (ImGui::Combo("Analysis Mode", &currentAnalysisMode, analysisModes, IM_ARRAYSIZE(analysisModes)))
    {
        mTerrainAnalysisMode = static_cast<TerrainAnalysisMode>(currentAnalysisMode);
    }
    
    if (mTerrainAnalysisMode == TerrainAnalysisMode::Slope)
    {
        ImGui::SliderFloat("Slope Threshold (deg)", &mSlopeThreshold, 10.0f, 80.0f);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Overlays");
    
    // Contour lines
    ImGui::Checkbox("Show Contour Lines", &mShowContourLines);
    if (mShowContourLines)
    {
        ImGui::SliderFloat("Contour Interval", &mContourInterval, 10.0f, 200.0f);
        ImGui::SliderFloat("Contour Width", &mContourLineWidth, 0.5f, 5.0f);
    }
    
    // Distance rings
    ImGui::Checkbox("Show Distance Rings", &mShowDistanceRings);
    if (mShowDistanceRings)
    {
        ImGui::SliderFloat("Ring Interval", &mDistanceRingInterval, 50.0f, 500.0f);
    }
    
    // Grid
    ImGui::Checkbox("Show Grid", &mShowGrid);
    if (mShowGrid)
    {
        ImGui::SliderFloat("Grid Size", &mGridSize, 25.0f, 250.0f);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Info");
    ImGui::Text("Mouse World Position: (%.1f, %.1f, %.1f)", mMouseWorldPosition[0], mMouseWorldPosition[1], mMouseWorldPosition[2]);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void LineOfSightAnalyzer::TerrainRenderer::Resize(int w, int h)
{
    mWidth = w;
    mHeight = h;

    DeleteFramebuffer();
    CreateFramebuffer();
}

void LineOfSightAnalyzer::TerrainRenderer::onMouseMoved(QMouseEvent* event)
{
    mFramebuffer->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    QVector4D MouseWorldPosition;
    glReadPixels(event->pos().x() * mDevicePixelRatio, mFramebuffer->height() - event->pos().y() * mDevicePixelRatio, 1, 1, GL_RGBA, GL_FLOAT, &MouseWorldPosition);
    mFramebuffer->release();

    if (MouseWorldPosition.x() != 0.0f) // 
    {
        mMouseWorldPosition = QVector3D(MouseWorldPosition.x(), MouseWorldPosition.y(), MouseWorldPosition.z());
    }       
}

void LineOfSightAnalyzer::TerrainRenderer::DeleteFramebuffer()
{
    if (mFramebuffer)
    {
        delete mFramebuffer;
        mFramebuffer = nullptr;
    }
}

void LineOfSightAnalyzer::TerrainRenderer::CreateFramebuffer()
{
    mFramebuffer = new QOpenGLFramebufferObject(mWidth, mHeight, mFramebufferFormat);
    mFramebuffer->addColorAttachment(mWidth, mHeight, GL_RGB32F);
    mFramebuffer->bind();
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    mFramebuffer->release();
}
