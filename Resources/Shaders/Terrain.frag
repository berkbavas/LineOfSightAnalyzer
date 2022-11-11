#version 330 core
layout(location = 0) out vec4 outFragColor;
layout(location = 1) out vec3 outFragWorldPosition;

uniform sampler2D heightMap;
uniform sampler2D losMap;

in vec2 fsTextureCoords;
in vec3 fsFragWorldPosition;

void main()
{
    outFragWorldPosition = fsFragWorldPosition;

    vec4 losColor = texture(losMap, fsTextureCoords);
    vec4 heightColor = texture(heightMap, fsTextureCoords);

    if (losColor.a > 0.5f)
        outFragColor = mix(heightColor, losColor, 0.75);
    else
        outFragColor = heightColor;
}
