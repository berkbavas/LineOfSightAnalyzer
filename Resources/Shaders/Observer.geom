#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 VPs[6];

in vec3 gs_FragWorldPosition[];
out vec4 fs_FragWorldPosition;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face;            // built-in variable that specifies to which face we render.
        for (int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            fs_FragWorldPosition = vec4(gs_FragWorldPosition[i], 1);
            gl_Position = VPs[face] * fs_FragWorldPosition;
            EmitVertex();
        }

        EndPrimitive();
    }
}
