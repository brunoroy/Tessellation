#version 400

layout(vertices = 3) out;
in vec4 vertexPosition[];
out vec4 controlPosition[];

uniform int innerTL;
uniform int outerTL;

#define INV_ID gl_InvocationID

void main()
{
    controlPosition[INV_ID] = vertexPosition[INV_ID];
    if (INV_ID == 0)
    {
        gl_TessLevelInner[0] = innerTL;
        gl_TessLevelOuter[0] = outerTL;
        gl_TessLevelOuter[1] = outerTL;
        gl_TessLevelOuter[2] = outerTL;
    }
}
