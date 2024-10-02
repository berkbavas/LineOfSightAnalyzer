#version 450 core

uniform sampler2D heightMap;
uniform samplerCube depthMap;
uniform vec3 observerPosition;
uniform float farPlane;
uniform float bias;

in vec2 fs_TextureCoords;
in vec3 fs_FragWorldPosition;

layout(location = 0) out vec4 out_FragColor;
layout(location = 1) out vec3 out_FragWorldPosition;

void main()
{
    // this is needed for fetching mouse world position
    out_FragWorldPosition = fs_FragWorldPosition;

    vec4 color = texture(heightMap, fs_TextureCoords);

    out_FragColor = vec4(color.xyz, 1.0f);

    vec3 fragmentToObserver = fs_FragWorldPosition - observerPosition;
    float closestDepth = texture(depthMap, fragmentToObserver).r * farPlane;
    float currentDepth = length(fragmentToObserver);

    if (currentDepth - bias * farPlane < closestDepth)
    {
        out_FragColor = mix(mix(vec4(0, 1, 0, 1), vec4(1, 0, 0, 1), currentDepth / farPlane), out_FragColor, 0.3f);
    }
}