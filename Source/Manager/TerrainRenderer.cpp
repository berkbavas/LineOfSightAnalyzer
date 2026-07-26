#include "TerrainRenderer.h"

#include "Manager/CameraManager.h"
#include "Manager/LineOfSightRenderer.h"

#include <imgui.h>

#include <QtImgui.h>

LineOfSightAnalyzer::TerrainRenderer::TerrainRenderer()
{
    initializeOpenGLFunctions();

    mFramebufferFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    mFramebufferFormat.setSamples(0);
    mFramebufferFormat.setTextureTarget(GL_TEXTURE_2D);

    mExtraColorAttachments = { GL_RGB32F }; // For mouse world position

    mTerrainShader = std::make_unique<Shader>("Terrain Shader");
    mTerrainShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
    mTerrainShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tcs");
    mTerrainShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tes");
    mTerrainShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Terrain.frag");
    mTerrainShader->Initialize();

    mTextureLoader = std::make_unique<TextureLoader>();

    Texture Texture = mTextureLoader->LoadTexture2D("Resources/HeightMaps/01.png");
    Texture.Name = "uHeightMap";
    Texture.Unit = 0;

    mTerrain = std::make_unique<Terrain>(Texture);

    mFramebuffer = std::make_unique<Framebuffer>(1, 1, mFramebufferFormat, mExtraColorAttachments);
}

void LineOfSightAnalyzer::TerrainRenderer::Render(Camera* pActiveCamera, float DevicePixelRatio, float Ifps)
{
    mFramebuffer->Bind();
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, mFramebuffer->GetWidth(), mFramebuffer->GetHeight());
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Enable wireframe mode if selected
    if (mWireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    mTerrainShader->Bind();
    mTerrainShader->SetUniform("uModelMatrix", QMatrix4x4());
    mTerrainShader->SetUniform("uViewMatrix", pActiveCamera->GetViewMatrix());
    mTerrainShader->SetUniform("uProjectionMatrix", pActiveCamera->GetProjectionMatrix());
    mTerrainShader->SetUniform("uMinimumElevation", mTerrain->GetMinElevation());
    mTerrainShader->SetUniform("uMaximumElevation", mTerrain->GetMaxElevation());
    mTerrainShader->SetUniform("uObserverPosition", mLineOfSightAnalyzer->GetObservers().at(0)->GetPosition());
    mTerrainShader->SetUniform("uFarPlane", mLineOfSightAnalyzer->GetObservers().at(0)->GetZFar());
    mTerrainShader->SetUniform("uBias", mBias);
    mTerrainShader->SetUniform("uColorScheme", static_cast<int>(mColorScheme));
    mTerrainShader->SetUniform("uVisibilityOpacity", mVisibilityOpacity);
    mTerrainShader->SetUniform("uShowLos", mShowLos ? 1 : 0);
    mTerrainShader->SetUniform("uTerrainColorMode", static_cast<int>(mTerrainColorMode));
    mTerrainShader->SetUniform("uTerrainAnalysisMode", static_cast<int>(mTerrainAnalysisMode));
    mTerrainShader->SetUniform("uSlopeThreshold", mSlopeThreshold);
    mTerrainShader->SetUniform("uObserverMode", static_cast<int>(mLineOfSightAnalyzer->GetObserverMode()));
    mTerrainShader->SetUniform("uConeAngle", mLineOfSightAnalyzer->GetConeAngle());
    mTerrainShader->SetUniform("uConeDirection", mLineOfSightAnalyzer->GetConeDirection());
    mTerrainShader->SetUniform("uConePitch", mLineOfSightAnalyzer->GetConePitch());
    mTerrainShader->SetUniform("uVerticalAngleMin", mLineOfSightAnalyzer->GetVerticalAngleMin());
    mTerrainShader->SetUniform("uVerticalAngleMax", mLineOfSightAnalyzer->GetVerticalAngleMax());
    mTerrainShader->SetUniform("uShowContourLines", mShowContourLines ? 1 : 0);
    mTerrainShader->SetUniform("uContourInterval", mContourInterval);
    mTerrainShader->SetUniform("uContourLineWidth", mContourLineWidth);
    mTerrainShader->SetUniform("uShowDistanceRings", mShowDistanceRings ? 1 : 0);
    mTerrainShader->SetUniform("uDistanceRingInterval", mDistanceRingInterval);
    mTerrainShader->SetUniform("uShowGrid", mShowGrid ? 1 : 0);
    mTerrainShader->SetUniform("uGridSize", mGridSize);

    const auto& TerrainTexture = mTerrain->GetTexture();
    const auto& DepthMapTexture = mLineOfSightAnalyzer->GetDepthMap();
    mTerrainShader->SetSampler(TerrainTexture.Name, TerrainTexture.Unit, TerrainTexture.Id, TerrainTexture.Target);
    mTerrainShader->SetSampler(DepthMapTexture.Name, DepthMapTexture.Unit, DepthMapTexture.Id, DepthMapTexture.Target);
    mTerrain->Render();
    mTerrainShader->Unbind();
    mFramebuffer->Release();

    // Restore fill mode
    if (mWireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void LineOfSightAnalyzer::TerrainRenderer::DrawGui()
{
    if (ImGui::CollapsingHeader("Terrain Settings"))
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Elevation");
        ImGui::SliderFloat("Min Elevation##RenderSettings", &mTerrain->GetMinElevation_NonConst(), 0, 100);
        ImGui::SliderFloat("Max Elevation##RenderSettings", &mTerrain->GetMaxElevation_NonConst(), mTerrain->GetMinElevation(), 1000);
        ImGui::SliderFloat("Bias##RenderSettings", &mBias, 0.0f, 0.1f);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Visualization");

        // LOS Toggle
        ImGui::Checkbox("Show Line of Sight", &mShowLos);

        // Color Scheme dropdown
        const char* ColorSchemes[] = { "Green-Red", "Blue-Yellow", "Heat Map", "Purple-Cyan" };
        int currentScheme = static_cast<int>(mColorScheme);
        if (ImGui::Combo("LOS Color Scheme", &currentScheme, ColorSchemes, IM_ARRAYSIZE(ColorSchemes)))
        {
            mColorScheme = static_cast<ColorScheme>(currentScheme);
        }

        // Visibility Opacity
        ImGui::SliderFloat("LOS Opacity", &mVisibilityOpacity, 0.0f, 1.0f);

        // Terrain Color Mode
        const char* TerrainModes[] = { "Texture", "Height-Based", "Grayscale", "Heightmap" };
        int currentTerrainMode = static_cast<int>(mTerrainColorMode);
        if (ImGui::Combo("Terrain Color Mode", &currentTerrainMode, TerrainModes, IM_ARRAYSIZE(TerrainModes)))
        {
            mTerrainColorMode = static_cast<TerrainColorMode>(currentTerrainMode);
        }

        // Wireframe toggle
        ImGui::Checkbox("Wireframe Mode", &mWireframeMode);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Terrain Analysis");

        // Terrain Analysis Mode
        const char* AnalysisModes[] = { "None", "Slope", "Aspect (Direction)", "Curvature" };
        int currentAnalysisMode = static_cast<int>(mTerrainAnalysisMode);
        if (ImGui::Combo("Analysis Mode", &currentAnalysisMode, AnalysisModes, IM_ARRAYSIZE(AnalysisModes)))
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
    }
}

void LineOfSightAnalyzer::TerrainRenderer::Resize(int Width, int Height)
{
    mFramebuffer->Resize(Width, Height);
}

bool LineOfSightAnalyzer::TerrainRenderer::OnMouseMoved(QMouseEvent* pEvent)
{
    mFramebuffer->Bind();
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    QVector4D MouseWorldPosition;
    glReadPixels(pEvent->pos().x(), mFramebuffer->GetHeight() - pEvent->pos().y(), 1, 1, GL_RGBA, GL_FLOAT, &MouseWorldPosition);
    mFramebuffer->Release();

    if (MouseWorldPosition.x() != 0.0f)
    {
        mMouseWorldPosition = QVector3D(MouseWorldPosition.x(), MouseWorldPosition.y(), MouseWorldPosition.z());
        return true;
    }

    return false;
}

void LineOfSightAnalyzer::TerrainRenderer::SetLineOfSightRenderer(LineOfSightRenderer* pLineOfSightAnalyzer)
{
    mLineOfSightAnalyzer = pLineOfSightAnalyzer;
}

LineOfSightAnalyzer::Framebuffer* LineOfSightAnalyzer::TerrainRenderer::GetFramebuffer() const
{
    return mFramebuffer.get();
}

LineOfSightAnalyzer::Terrain* LineOfSightAnalyzer::TerrainRenderer::GetTerrain() const
{
    return mTerrain.get();
}

const QVector3D& LineOfSightAnalyzer::TerrainRenderer::GetMouseWorldPosition() const
{
    return mMouseWorldPosition;
}

void LineOfSightAnalyzer::TerrainRenderer::BlitToDefaultFramebuffer(int Width, int Height, float DevicePixelRatio)
{
    QOpenGLFramebufferObject::bindDefault();
    glViewport(0, 0, Width * DevicePixelRatio, Height * DevicePixelRatio);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    QOpenGLFramebufferObject::blitFramebuffer(nullptr, // Blit to the default framebuffer
                                              QRect(0, 0, Width * DevicePixelRatio, Height * DevicePixelRatio),
                                              mFramebuffer->GetFramebufferObject(),
                                              mFramebuffer->GetViewport(),
                                              GL_COLOR_BUFFER_BIT,
                                              GL_NEAREST,
                                              0,
                                              0);
}
