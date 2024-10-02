#version 450 core

in vec4 fs_FragWorldPosition;

uniform vec3 observerPosition;
uniform float farPlane;

void main()
{
    gl_FragDepth = length(fs_FragWorldPosition.xyz - observerPosition) / farPlane;
}
