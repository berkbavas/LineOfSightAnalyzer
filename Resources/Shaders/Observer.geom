#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 uVPs[6];

in vec3 gsFragWorldPosition[];
out vec4 fsFragWorldPosition;

void main()
{
    for (int Face = 0; Face < 6; ++Face)
    {
        gl_Layer = Face;            // Built-in variable that specifies to which face we render.
        for (int Index = 0; Index < 3; ++Index) // For each triangle's vertices
        {
            fsFragWorldPosition = vec4(gsFragWorldPosition[Index], 1.0f);
            gl_Position = uVPs[Face] * fsFragWorldPosition;
            EmitVertex();
        }

        EndPrimitive();
    }
}
