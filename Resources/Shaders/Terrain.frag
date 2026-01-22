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

void main()
{
    // This is needed for fetching mouse world position
    out_FragWorldPosition = fs_FragWorldPosition;

    vec4 terrainColor = getTerrainColor(fs_TextureCoords, fs_FragWorldPosition);
    out_FragColor = terrainColor;

    if (showLOS)
    {
        vec3 fragmentToObserver = fs_FragWorldPosition - observerPosition;
        float closestDepth = texture(depthMap, fragmentToObserver).r * farPlane;
        float currentDepth = length(fragmentToObserver);

        if (currentDepth - bias * farPlane < closestDepth)
        {
            float distanceRatio = currentDepth / farPlane;
            vec4 losColor = getVisibleColor(distanceRatio);
            out_FragColor = mix(losColor, terrainColor, visibilityOpacity);
        }
    }
}