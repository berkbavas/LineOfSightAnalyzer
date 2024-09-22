#version 330 core
layout (location = 0) out vec4 outFragColor;

uniform samplerCube depthMap;

in vec3 fsDirection;

void main()
{
    outFragColor = texture(depthMap, fsDirection);
}
