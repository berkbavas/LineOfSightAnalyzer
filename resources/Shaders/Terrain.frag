#version 330 core
layout(location = 0) out vec4 outFragColor;
layout(location = 1) out vec3 outFragWorldPosition;

uniform sampler2D heightMap;
uniform samplerCube depthMap;
uniform vec3 observerPosition;
uniform float farPlane;
uniform float maxDistance;
uniform float bias = 0.01;

in vec2 fsTextureCoords;
in vec3 fsFragWorldPosition;

void main()
{
    outFragWorldPosition = fsFragWorldPosition;
    outFragColor = texture(heightMap, fsTextureCoords);

    vec3 fragToObserver = fsFragWorldPosition - observerPosition;
    float closestDepth = texture(depthMap, fragToObserver).r * farPlane;
    float currentDepth = length(fragToObserver);

    if(currentDepth - bias * farPlane < closestDepth)
        outFragColor = mix(mix(vec4(0,1,0,1), vec4(1,0,0,1), currentDepth / maxDistance),outFragColor, 0.3f) ;
}
