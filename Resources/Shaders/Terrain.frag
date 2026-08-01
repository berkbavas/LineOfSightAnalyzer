#version 450 core

uniform sampler2D uHeightMap;
uniform samplerCube uDepthMap;
uniform vec3 uObserverPosition;
uniform float uFarPlane;
uniform float uBias;

// Visualization settings
uniform int uColorScheme;         // 0: Green-Red, 1: Blue-Yellow, 2: Heat Map, 3: Purple-Cyan
uniform float uVisibilityOpacity; // How much the LOS overlay blends with terrain
uniform bool uShowLos;            // Toggle LOS visualization
uniform int uTerrainColorMode;    // 0: Texture, 1: Height-based, 2: Grayscale, 3: Normals
uniform float uMinimumElevation;
uniform float uMaximumElevation;

// Terrain analysis settings
uniform int uTerrainAnalysisMode; // 0: None, 1: Slope, 2: Aspect, 3: Curvature
uniform float uSlopeThreshold;    // Degrees threshold for slope highlighting

// Observer mode settings
uniform int uObserverMode;       // 0: FullSphere, 1: Hemisphere, 2: DirectionalCone
uniform float uConeAngle;        // Half-angle for cone mode (degrees)
uniform float uConeDirection;    // Horizontal direction (degrees, 0=North)
uniform float uConePitch;        // Vertical pitch (degrees)
uniform float uVerticalAngleMin; // Min vertical angle
uniform float uVerticalAngleMax; // Max vertical angle

// Overlay settings
uniform bool uShowContourLines;
uniform float uContourInterval;
uniform float uContourLineWidth;
uniform bool uShowDistanceRings;
uniform float uDistanceRingInterval;
uniform bool uShowGrid;
uniform float uGridSize;

in vec3 fsFragWorldPosition;
in vec2 fsTextureCoords;

layout(location = 0) out vec4 oFragColor;
layout(location = 1) out vec3 oFragWorldPosition;

// Color scheme functions
vec4 GetVisibleColor(float DistanceRatio)
{
    switch (uColorScheme)
    {
    case 0: // Green to Red (classic)
        return mix(vec4(0, 1, 0, 1), vec4(1, 0, 0, 1), DistanceRatio);
    case 1: // Blue to Yellow
        return mix(vec4(0.2, 0.4, 1, 1), vec4(1, 1, 0, 1), DistanceRatio);
    case 2: // Heat Map (Blue -> Cyan -> Green -> Yellow -> Red)
    {
        float T = DistanceRatio * 4.0;
        if (T < 1.0)
            return mix(vec4(0, 0, 1, 1), vec4(0, 1, 1, 1), T);
        else if (T < 2.0)
            return mix(vec4(0, 1, 1, 1), vec4(0, 1, 0, 1), T - 1.0);
        else if (T < 3.0)
            return mix(vec4(0, 1, 0, 1), vec4(1, 1, 0, 1), T - 2.0);
        else
            return mix(vec4(1, 1, 0, 1), vec4(1, 0, 0, 1), T - 3.0);
    }
    case 3: // Purple to Cyan
        return mix(vec4(0.8, 0.2, 1, 1), vec4(0, 1, 1, 1), DistanceRatio);
    default:
        return mix(vec4(0, 1, 0, 1), vec4(1, 0, 0, 1), DistanceRatio);
    }
}

vec4 GetTerrainColor(vec2 TexCoords, vec3 WorldPos)
{
    const vec4 TexColor = texture(uHeightMap, TexCoords);

    switch (uTerrainColorMode)
    {
    case 0: // Original texture
        return TexColor;
    case 1: // Height-based coloring
    {
        float HeightNorm = (WorldPos.y - uMinimumElevation) / (uMaximumElevation - uMinimumElevation);
        HeightNorm = clamp(HeightNorm, 0.0, 1.0);

        // Terrain color gradient: water blue -> grass green -> mountain brown -> snow white
        if (HeightNorm < 0.2)
            return mix(vec4(0.1, 0.3, 0.6, 1), vec4(0.2, 0.5, 0.2, 1), HeightNorm / 0.2);
        else if (HeightNorm < 0.5)
            return mix(vec4(0.2, 0.5, 0.2, 1), vec4(0.4, 0.3, 0.2, 1), (HeightNorm - 0.2) / 0.3);
        else if (HeightNorm < 0.8)
            return mix(vec4(0.4, 0.3, 0.2, 1), vec4(0.5, 0.5, 0.5, 1), (HeightNorm - 0.5) / 0.3);
        else
            return mix(vec4(0.5, 0.5, 0.5, 1), vec4(1, 1, 1, 1), (HeightNorm - 0.8) / 0.2);
    }
    case 2: // Grayscale
    {
        float Gray = dot(TexColor.rgb, vec3(0.299, 0.587, 0.114));
        return vec4(Gray, Gray, Gray, 1);
    }
    case 3: // Heightmap visualization (black to white based on height)
    {
        float HeightNorm = (WorldPos.y - uMinimumElevation) / (uMaximumElevation - uMinimumElevation);
        HeightNorm = clamp(HeightNorm, 0.0, 1.0);
        return vec4(HeightNorm, HeightNorm, HeightNorm, 1);
    }
    default:
        return TexColor;
    }
}

// Calculate terrain slope at this point (returns angle in degrees)
float CalculateSlope(vec2 TexCoords)
{
    vec2 TexelSize = 1.0 / textureSize(uHeightMap, 0);

    float HL = texture(uHeightMap, TexCoords - vec2(TexelSize.x, 0)).r;
    float HR = texture(uHeightMap, TexCoords + vec2(TexelSize.x, 0)).r;
    float HD = texture(uHeightMap, TexCoords - vec2(0, TexelSize.y)).r;
    float HU = texture(uHeightMap, TexCoords + vec2(0, TexelSize.y)).r;

    float DX = (HR - HL) * (uMaximumElevation - uMinimumElevation);
    float DZ = (HU - HD) * (uMaximumElevation - uMinimumElevation);

    float Slope = atan(sqrt(DX * DX + DZ * DZ));
    return degrees(Slope);
}

// Calculate terrain aspect (direction the slope faces, in degrees 0-360)
float CalculateAspect(vec2 TexCoords)
{
    vec2 TexelSize = 1.0 / textureSize(uHeightMap, 0);

    float HL = texture(uHeightMap, TexCoords - vec2(TexelSize.x, 0)).r;
    float HR = texture(uHeightMap, TexCoords + vec2(TexelSize.x, 0)).r;
    float HD = texture(uHeightMap, TexCoords - vec2(0, TexelSize.y)).r;
    float HU = texture(uHeightMap, TexCoords + vec2(0, TexelSize.y)).r;

    float DX = (HR - HL);
    float DZ = (HU - HD);

    float Aspect = degrees(atan(DZ, DX));
    if (Aspect < 0)
        Aspect += 360.0;
    return Aspect;
}

// Calculate terrain curvature (positive = convex, negative = concave)
float CalculateCurvature(vec2 TexCoords)
{
    vec2 TexelSize = 1.0 / textureSize(uHeightMap, 0);

    float HC = texture(uHeightMap, TexCoords).r;
    float HL = texture(uHeightMap, TexCoords - vec2(TexelSize.x, 0)).r;
    float HR = texture(uHeightMap, TexCoords + vec2(TexelSize.x, 0)).r;
    float HD = texture(uHeightMap, TexCoords - vec2(0, TexelSize.y)).r;
    float HU = texture(uHeightMap, TexCoords + vec2(0, TexelSize.y)).r;

    float Curvature = (HL + HR + HD + HU) / 4.0 - HC;
    return Curvature * 100.0; // Scale for visibility
}

// Get color for terrain analysis mode
vec4 GetTerrainAnalysisColor(vec2 TexCoords, vec4 BaseColor)
{
    switch (uTerrainAnalysisMode)
    {
    case 1: // Slope analysis
    {
        float Slope = CalculateSlope(TexCoords);
        float SlopeNorm = clamp(Slope / 90.0, 0.0, 1.0);

        // Color gradient: green (flat) -> yellow -> orange -> red (steep)
        vec4 SlopeColor;
        if (Slope < uSlopeThreshold * 0.5)
            SlopeColor = mix(vec4(0.2, 0.8, 0.2, 1), vec4(1, 1, 0, 1), Slope / (uSlopeThreshold * 0.5));
        else if (Slope < uSlopeThreshold)
            SlopeColor = mix(vec4(1, 1, 0, 1), vec4(1, 0.5, 0, 1), (Slope - uSlopeThreshold * 0.5) / (uSlopeThreshold * 0.5));
        else
            SlopeColor = mix(vec4(1, 0.5, 0, 1), vec4(1, 0, 0, 1), min((Slope - uSlopeThreshold) / uSlopeThreshold, 1.0));

        return mix(SlopeColor, BaseColor, 0.3);
    }
    case 2: // Aspect analysis (direction facing)
    {
        float Aspect = CalculateAspect(TexCoords);

        // Color wheel based on direction
        float Hue = Aspect / 360.0;
        vec3 AspectColor;
        if (Hue < 0.166)
            AspectColor = mix(vec3(1, 0, 0), vec3(1, 1, 0), Hue * 6.0);
        else if (Hue < 0.333)
            AspectColor = mix(vec3(1, 1, 0), vec3(0, 1, 0), (Hue - 0.166) * 6.0);
        else if (Hue < 0.5)
            AspectColor = mix(vec3(0, 1, 0), vec3(0, 1, 1), (Hue - 0.333) * 6.0);
        else if (Hue < 0.666)
            AspectColor = mix(vec3(0, 1, 1), vec3(0, 0, 1), (Hue - 0.5) * 6.0);
        else if (Hue < 0.833)
            AspectColor = mix(vec3(0, 0, 1), vec3(1, 0, 1), (Hue - 0.666) * 6.0);
        else
            AspectColor = mix(vec3(1, 0, 1), vec3(1, 0, 0), (Hue - 0.833) * 6.0);

        return mix(vec4(AspectColor, 1.0), BaseColor, 0.3);
    }
    case 3: // Curvature analysis
    {
        float Curvature = CalculateCurvature(TexCoords);

        // Blue for concave (valleys), red for convex (ridges)
        vec4 CurvColor;
        if (Curvature < 0)
            CurvColor = mix(vec4(1, 1, 1, 1), vec4(0, 0.3, 1, 1), min(-Curvature, 1.0));
        else
            CurvColor = mix(vec4(1, 1, 1, 1), vec4(1, 0.3, 0, 1), min(Curvature, 1.0));

        return mix(CurvColor, BaseColor, 0.3);
    }
    default:
        return BaseColor;
    }
}

// Check if point is visible based on observer mode
bool IsVisibleInObserverMode(vec3 FragmentToObserver)
{
    float Distance = length(FragmentToObserver);
    vec3 Dir = normalize(-FragmentToObserver); // Direction from observer to fragment

    // Calculate vertical angle (pitch)
    float VerticalAngle = degrees(asin(Dir.y));

    // Calculate horizontal angle (yaw)
    float HorizontalAngle = degrees(atan(Dir.x, Dir.z));
    if (HorizontalAngle < 0)
        HorizontalAngle += 360.0;

    switch (uObserverMode)
    {
    case 0: // FullSphere - always visible (within distance)
        return true;

    case 1: // Hemisphere - only above horizon
        return VerticalAngle >= uVerticalAngleMin && VerticalAngle <= uVerticalAngleMax;

    case 2: // DirectionalCone
    {
        // Calculate angle between fragment direction and cone direction
        float ConeYaw = uConeDirection;
        float ConePitchRad = radians(uConePitch);
        vec3 ConeDir = vec3(sin(radians(ConeYaw)) * cos(ConePitchRad), sin(ConePitchRad), cos(radians(ConeYaw)) * cos(ConePitchRad));

        float AngleToCone = degrees(acos(clamp(dot(Dir, ConeDir), -1.0, 1.0)));
        return AngleToCone <= uConeAngle;
    }
    default:
        return true;
    }
}

// Draw overlay effects
vec4 ApplyOverlays(vec4 Color, vec3 WorldPos, vec2 TexCoords)
{
    vec4 Result = Color;

    // Contour lines
    if (uShowContourLines)
    {
        float Height = WorldPos.y;
        float ContourMod = mod(Height, uContourInterval);
        float ContourDist = min(ContourMod, uContourInterval - ContourMod);

        if (ContourDist < uContourLineWidth)
        {
            float T = ContourDist / uContourLineWidth;
            Result = mix(vec4(0.1, 0.1, 0.1, 1), Result, T);
        }
    }

    // Distance rings from observer
    if (uShowDistanceRings)
    {
        float Dist = length(WorldPos.xz - uObserverPosition.xz);
        float RingMod = mod(Dist, uDistanceRingInterval);
        float RingDist = min(RingMod, uDistanceRingInterval - RingMod);

        if (RingDist < 2.0)
        {
            float T = RingDist / 2.0;
            Result = mix(vec4(1, 1, 0, 1), Result, T);
        }
    }

    // Grid overlay
    if (uShowGrid)
    {
        float GridModX = mod(WorldPos.x + uGridSize * 0.5, uGridSize);
        float GridModZ = mod(WorldPos.z + uGridSize * 0.5, uGridSize);
        float GridDistX = min(GridModX, uGridSize - GridModX);
        float GridDistZ = min(GridModZ, uGridSize - GridModZ);

        if (GridDistX < 1.0 || GridDistZ < 1.0)
        {
            float T = min(GridDistX, GridDistZ);
            Result = mix(vec4(0.5, 0.5, 0.5, 1), Result, T);
        }
    }

    return Result;
}

void main()
{
    // This is needed for fetching mouse world position
    oFragWorldPosition = fsFragWorldPosition;

    vec4 TerrainColor = GetTerrainColor(fsTextureCoords, fsFragWorldPosition);

    // Apply terrain analysis if enabled
    if (uTerrainAnalysisMode > 0)
    {
        TerrainColor = GetTerrainAnalysisColor(fsTextureCoords, TerrainColor);
    }

    oFragColor = TerrainColor;

    if (uShowLos)
    {
        vec3 FragmentToObserver = fsFragWorldPosition - uObserverPosition;
        float ClosestDepth = texture(uDepthMap, normalize(FragmentToObserver)).r * uFarPlane;
        float CurrentDepth = length(FragmentToObserver);

        if (CurrentDepth - uBias * uFarPlane < ClosestDepth)
        {
            // Check if visible in current observer mode
            if (IsVisibleInObserverMode(FragmentToObserver))
            {
                float DistanceRatio = CurrentDepth / uFarPlane;
                vec4 LosColor = GetVisibleColor(DistanceRatio);
                oFragColor = mix(LosColor, TerrainColor, uVisibilityOpacity);
            }
        }
    }

    // Apply overlays on top
    oFragColor = ApplyOverlays(oFragColor, fsFragWorldPosition, fsTextureCoords);
}