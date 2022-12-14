#version 330 core
layout(location = 0) in vec2 position;

uniform mat4 IVP; // Inverse rotation-projection matrix

out vec3 fsDirection;

void main()
{
    vec4 clipPos = vec4(position, -1.0f, 1.0f);
    vec4 viewPos  = IVP * clipPos;
    fsDirection = normalize(viewPos.xyz);
    gl_Position = vec4(position, -1.0f, 1.0f);
}
