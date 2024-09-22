#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoords;

uniform float minElevation;
uniform float maxElevation;
uniform sampler2D heightMap;

void main()
{
    float height = minElevation + (maxElevation - minElevation) * texture(heightMap, textureCoords).r;
    gl_Position = vec4(position.x, position.y + height, position.z, 1);
}

