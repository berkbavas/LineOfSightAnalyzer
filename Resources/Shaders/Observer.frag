#version 450 core

in vec4 fsFragWorldPosition;

uniform vec3 uObserverPosition;
uniform float uFarPlane;

void main()
{
    gl_FragDepth = length(fsFragWorldPosition.xyz - uObserverPosition) / uFarPlane;
}
