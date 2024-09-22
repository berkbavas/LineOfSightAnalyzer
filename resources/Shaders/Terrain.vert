#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoords;

uniform mat4 VP;
uniform float minElevation = 0.0f;
uniform float maxElevation = 100.0f;
uniform sampler2D heightMap;

out vec2 fsTextureCoords;
out vec3 fsFragWorldPosition;

void main()
{
    float height = minElevation + (maxElevation - minElevation) * texture(heightMap, textureCoords).r;
    fsFragWorldPosition = vec3(position.x, position.y + height, position.z);
    gl_Position = VP * vec4(fsFragWorldPosition, 1);
    fsTextureCoords = textureCoords;
}

