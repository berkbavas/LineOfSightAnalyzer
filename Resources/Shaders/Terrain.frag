#version 450 core

uniform sampler2D heightMap;
uniform samplerCube depthMap;
uniform vec3 observerPosition;
uniform float farPlane;
uniform float bias;

// Visualization settings
uniform int colorScheme;        // 0: Green-Red, 1: Blue-Yellow, 2: Heat Map, 3: Purple-Cyan
uniform float visibilityOpacity; // How much the LOS overlay blends with terrain
uniform bool showLOS;           // Toggle LOS visualization
uniform int terrainColorMode;   // 0: Texture, 1: Height-based, 2: Grayscale, 3: Normals
uniform float minElevation;
uniform float maxElevation;

// Terrain analysis settings
uniform int terrainAnalysisMode;  // 0: None, 1: Slope, 2: Aspect, 3: Curvature
uniform float slopeThreshold;     // Degrees threshold for slope highlighting

// Observer mode settings
uniform int observerMode;         // 0: FullSphere, 1: Hemisphere, 2: DirectionalCone
uniform float coneAngle;          // Half-angle for cone mode (degrees)
uniform float coneDirection;      // Horizontal direction (degrees, 0=North)
uniform float conePitch;          // Vertical pitch (degrees)
uniform float verticalAngleMin;   // Min vertical angle
uniform float verticalAngleMax;   // Max vertical angle

// Overlay settings
uniform bool showContourLines;
uniform float contourInterval;
uniform float contourLineWidth;
uniform bool showDistanceRings;
uniform float distanceRingInterval;
uniform bool showGrid;
uniform float gridSize;

in vec2 fs_TextureCoords;
in vec3 fs_FragWorldPosition;

layout(location = 0) out vec4 out_FragColor;
layout(location = 1) out vec3 out_FragWorldPosition;

// Color scheme functions
vec4 getVisibleColor(float distanceRatio)
{
    switch(colorScheme)
    {
        case 0: // Green to Red (classic)
            return mix(vec4(0, 1, 0, 1), vec4(1, 0, 0, 1), distanceRatio);
        case 1: // Blue to Yellow
            return mix(vec4(0.2, 0.4, 1, 1), vec4(1, 1, 0, 1), distanceRatio);
        case 2: // Heat Map (Blue -> Cyan -> Green -> Yellow -> Red)
        {
            float t = distanceRatio * 4.0;
            if (t < 1.0) return mix(vec4(0, 0, 1, 1), vec4(0, 1, 1, 1), t);
            else if (t < 2.0) return mix(vec4(0, 1, 1, 1), vec4(0, 1, 0, 1), t - 1.0);
            else if (t < 3.0) return mix(vec4(0, 1, 0, 1), vec4(1, 1, 0, 1), t - 2.0);
            else return mix(vec4(1, 1, 0, 1), vec4(1, 0, 0, 1), t - 3.0);
        }
        case 3: // Purple to Cyan
            return mix(vec4(0.8, 0.2, 1, 1), vec4(0, 1, 1, 1), distanceRatio);
        default:
            return mix(vec4(0, 1, 0, 1), vec4(1, 0, 0, 1), distanceRatio);
    }
}

vec4 getTerrainColor(vec2 texCoords, vec3 worldPos)
{
    vec4 texColor = texture(heightMap, texCoords);
    
    switch(terrainColorMode)
    {
        case 0: // Original texture
            return texColor;
        case 1: // Height-based coloring
        {
            float heightNorm = (worldPos.y - minElevation) / (maxElevation - minElevation);
            heightNorm = clamp(heightNorm, 0.0, 1.0);
            
            // Terrain color gradient: water blue -> grass green -> mountain brown -> snow white
            if (heightNorm < 0.2)
                return mix(vec4(0.1, 0.3, 0.6, 1), vec4(0.2, 0.5, 0.2, 1), heightNorm / 0.2);
            else if (heightNorm < 0.5)
                return mix(vec4(0.2, 0.5, 0.2, 1), vec4(0.4, 0.3, 0.2, 1), (heightNorm - 0.2) / 0.3);
            else if (heightNorm < 0.8)
                return mix(vec4(0.4, 0.3, 0.2, 1), vec4(0.5, 0.5, 0.5, 1), (heightNorm - 0.5) / 0.3);
            else
                return mix(vec4(0.5, 0.5, 0.5, 1), vec4(1, 1, 1, 1), (heightNorm - 0.8) / 0.2);
        }
        case 2: // Grayscale
        {
            float gray = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
            return vec4(gray, gray, gray, 1);
        }
        case 3: // Heightmap visualization (black to white based on height)
        {
            float heightNorm = (worldPos.y - minElevation) / (maxElevation - minElevation);
            heightNorm = clamp(heightNorm, 0.0, 1.0);
            return vec4(heightNorm, heightNorm, heightNorm, 1);
        }
        default:
            return texColor;
    }
}

// Calculate terrain slope at this point (returns angle in degrees)
float calculateSlope(vec2 texCoords)
{
    vec2 texelSize = 1.0 / textureSize(heightMap, 0);
    
    float hL = texture(heightMap, texCoords - vec2(texelSize.x, 0)).r;
    float hR = texture(heightMap, texCoords + vec2(texelSize.x, 0)).r;
    float hD = texture(heightMap, texCoords - vec2(0, texelSize.y)).r;
    float hU = texture(heightMap, texCoords + vec2(0, texelSize.y)).r;
    
    float dX = (hR - hL) * (maxElevation - minElevation);
    float dZ = (hU - hD) * (maxElevation - minElevation);
    
    float slope = atan(sqrt(dX * dX + dZ * dZ));
    return degrees(slope);
}

// Calculate terrain aspect (direction the slope faces, in degrees 0-360)
float calculateAspect(vec2 texCoords)
{
    vec2 texelSize = 1.0 / textureSize(heightMap, 0);
    
    float hL = texture(heightMap, texCoords - vec2(texelSize.x, 0)).r;
    float hR = texture(heightMap, texCoords + vec2(texelSize.x, 0)).r;
    float hD = texture(heightMap, texCoords - vec2(0, texelSize.y)).r;
    float hU = texture(heightMap, texCoords + vec2(0, texelSize.y)).r;
    
    float dX = (hR - hL);
    float dZ = (hU - hD);
    
    float aspect = degrees(atan(dZ, dX));
    if (aspect < 0) aspect += 360.0;
    return aspect;
}

// Calculate terrain curvature (positive = convex, negative = concave)
float calculateCurvature(vec2 texCoords)
{
    vec2 texelSize = 1.0 / textureSize(heightMap, 0);
    
    float hC = texture(heightMap, texCoords).r;
    float hL = texture(heightMap, texCoords - vec2(texelSize.x, 0)).r;
    float hR = texture(heightMap, texCoords + vec2(texelSize.x, 0)).r;
    float hD = texture(heightMap, texCoords - vec2(0, texelSize.y)).r;
    float hU = texture(heightMap, texCoords + vec2(0, texelSize.y)).r;
    
    float curvature = (hL + hR + hD + hU) / 4.0 - hC;
    return curvature * 100.0; // Scale for visibility
}

// Get color for terrain analysis mode
vec4 getTerrainAnalysisColor(vec2 texCoords, vec4 baseColor)
{
    switch(terrainAnalysisMode)
    {
        case 1: // Slope analysis
        {
            float slope = calculateSlope(texCoords);
            float slopeNorm = clamp(slope / 90.0, 0.0, 1.0);
            
            // Color gradient: green (flat) -> yellow -> orange -> red (steep)
            vec4 slopeColor;
            if (slope < slopeThreshold * 0.5)
                slopeColor = mix(vec4(0.2, 0.8, 0.2, 1), vec4(1, 1, 0, 1), slope / (slopeThreshold * 0.5));
            else if (slope < slopeThreshold)
                slopeColor = mix(vec4(1, 1, 0, 1), vec4(1, 0.5, 0, 1), (slope - slopeThreshold * 0.5) / (slopeThreshold * 0.5));
            else
                slopeColor = mix(vec4(1, 0.5, 0, 1), vec4(1, 0, 0, 1), min((slope - slopeThreshold) / slopeThreshold, 1.0));
            
            return mix(slopeColor, baseColor, 0.3);
        }
        case 2: // Aspect analysis (direction facing)
        {
            float aspect = calculateAspect(texCoords);
            
            // Color wheel based on direction
            float hue = aspect / 360.0;
            vec3 aspectColor;
            if (hue < 0.166) aspectColor = mix(vec3(1,0,0), vec3(1,1,0), hue * 6.0);
            else if (hue < 0.333) aspectColor = mix(vec3(1,1,0), vec3(0,1,0), (hue - 0.166) * 6.0);
            else if (hue < 0.5) aspectColor = mix(vec3(0,1,0), vec3(0,1,1), (hue - 0.333) * 6.0);
            else if (hue < 0.666) aspectColor = mix(vec3(0,1,1), vec3(0,0,1), (hue - 0.5) * 6.0);
            else if (hue < 0.833) aspectColor = mix(vec3(0,0,1), vec3(1,0,1), (hue - 0.666) * 6.0);
            else aspectColor = mix(vec3(1,0,1), vec3(1,0,0), (hue - 0.833) * 6.0);
            
            return mix(vec4(aspectColor, 1.0), baseColor, 0.3);
        }
        case 3: // Curvature analysis
        {
            float curvature = calculateCurvature(texCoords);
            
            // Blue for concave (valleys), red for convex (ridges)
            vec4 curvColor;
            if (curvature < 0)
                curvColor = mix(vec4(1, 1, 1, 1), vec4(0, 0.3, 1, 1), min(-curvature, 1.0));
            else
                curvColor = mix(vec4(1, 1, 1, 1), vec4(1, 0.3, 0, 1), min(curvature, 1.0));
            
            return mix(curvColor, baseColor, 0.3);
        }
        default:
            return baseColor;
    }
}

// Check if point is visible based on observer mode
bool isVisibleInObserverMode(vec3 fragmentToObserver)
{
    float distance = length(fragmentToObserver);
    vec3 dir = normalize(-fragmentToObserver); // Direction from observer to fragment
    
    // Calculate vertical angle (pitch)
    float verticalAngle = degrees(asin(dir.y));
    
    // Calculate horizontal angle (yaw)
    float horizontalAngle = degrees(atan(dir.x, dir.z));
    if (horizontalAngle < 0) horizontalAngle += 360.0;
    
    switch(observerMode)
    {
        case 0: // FullSphere - always visible (within distance)
            return true;
            
        case 1: // Hemisphere - only above horizon
            return verticalAngle >= verticalAngleMin && verticalAngle <= verticalAngleMax;
            
        case 2: // DirectionalCone
        {
            // Calculate angle between fragment direction and cone direction
            float coneYaw = coneDirection;
            float conePitchRad = radians(conePitch);
            vec3 coneDir = vec3(sin(radians(coneYaw)) * cos(conePitchRad), 
                                sin(conePitchRad), 
                                cos(radians(coneYaw)) * cos(conePitchRad));
            
            float angleToCone = degrees(acos(clamp(dot(dir, coneDir), -1.0, 1.0)));
            return angleToCone <= coneAngle;
        }
        default:
            return true;
    }
}

// Draw overlay effects
vec4 applyOverlays(vec4 color, vec3 worldPos, vec2 texCoords)
{
    vec4 result = color;
    
    // Contour lines
    if (showContourLines)
    {
        float height = worldPos.y;
        float contourMod = mod(height, contourInterval);
        float contourDist = min(contourMod, contourInterval - contourMod);
        
        if (contourDist < contourLineWidth)
        {
            float t = contourDist / contourLineWidth;
            result = mix(vec4(0.1, 0.1, 0.1, 1), result, t);
        }
    }
    
    // Distance rings from observer
    if (showDistanceRings)
    {
        float dist = length(worldPos.xz - observerPosition.xz);
        float ringMod = mod(dist, distanceRingInterval);
        float ringDist = min(ringMod, distanceRingInterval - ringMod);
        
        if (ringDist < 2.0)
        {
            float t = ringDist / 2.0;
            result = mix(vec4(1, 1, 0, 1), result, t);
        }
    }
    
    // Grid overlay
    if (showGrid)
    {
        float gridModX = mod(worldPos.x + gridSize * 0.5, gridSize);
        float gridModZ = mod(worldPos.z + gridSize * 0.5, gridSize);
        float gridDistX = min(gridModX, gridSize - gridModX);
        float gridDistZ = min(gridModZ, gridSize - gridModZ);
        
        if (gridDistX < 1.0 || gridDistZ < 1.0)
        {
            float t = min(gridDistX, gridDistZ);
            result = mix(vec4(0.5, 0.5, 0.5, 1), result, t);
        }
    }
    
    return result;
}

void main()
{
    // This is needed for fetching mouse world position
    out_FragWorldPosition = fs_FragWorldPosition;

    vec4 terrainColor = getTerrainColor(fs_TextureCoords, fs_FragWorldPosition);
    
    // Apply terrain analysis if enabled
    if (terrainAnalysisMode > 0)
    {
        terrainColor = getTerrainAnalysisColor(fs_TextureCoords, terrainColor);
    }
    
    out_FragColor = terrainColor;

    if (showLOS)
    {
        vec3 fragmentToObserver = fs_FragWorldPosition - observerPosition;
        float closestDepth = texture(depthMap, fragmentToObserver).r * farPlane;
        float currentDepth = length(fragmentToObserver);

        if (currentDepth - bias * farPlane < closestDepth)
        {
            // Check if visible in current observer mode
            if (isVisibleInObserverMode(fragmentToObserver))
            {
                float distanceRatio = currentDepth / farPlane;
                vec4 losColor = getVisibleColor(distanceRatio);
                out_FragColor = mix(losColor, terrainColor, visibilityOpacity);
            }
        }
    }
    
    // Apply overlays on top
    out_FragColor = applyOverlays(out_FragColor, fs_FragWorldPosition, fs_TextureCoords);
}