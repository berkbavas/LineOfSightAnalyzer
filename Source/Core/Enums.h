#pragma once

namespace LineOfSightAnalyzer
{
    enum class ViewDirection
    {
        Front,
        Back,
        Up,
        Down,
        Left,
        Right
    };

    enum FramebufferType
    {
        Multisample,
        Singlesample,
    };

    enum class ColorScheme : int
    {
        GreenRed = 0,
        BlueYellow = 1,
        HeatMap = 2,
        PurpleCyan = 3
    };

    enum class TerrainColorMode : int
    {
        Texture = 0,
        HeightBased = 1,
        Grayscale = 2,
        HeightmapVis = 3
    };

    enum class TerrainAnalysisMode : int
    {
        None = 0,
        Slope = 1,
        Aspect = 2,
        Curvature = 3
    };

    enum class ObserverMode : int
    {
        FullSphere = 0,     // 360° horizontal, 180° vertical (default)
        Hemisphere = 1,     // 360° horizontal, 90° vertical (only looks up/around, not below)
        DirectionalCone = 2 // Limited FOV cone in a specific direction
    };
}