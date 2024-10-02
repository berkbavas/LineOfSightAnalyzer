#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoords;

out vec2 tcs_TextureCoords;

void main()
{
    // convert XYZ vertex to XYZW homogeneous coordinate
    gl_Position = vec4(position, 1.0);

    // pass texture coordinate though
    tcs_TextureCoords = textureCoords;
}