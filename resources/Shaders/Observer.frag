#version 330 core
in vec4 fsFragWorldPosition;

uniform vec3 observerPosition;
uniform float farPlane;

void main()
{
     gl_FragDepth = length(fsFragWorldPosition.xyz - observerPosition) / farPlane;
}
