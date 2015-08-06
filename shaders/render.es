#version 400

layout(triangles, equal_spacing, cw) in;
in vec4 controlPosition[];
out vec4 evaluationPosition;
out vec3 patchDistance;

uniform mat4 mvp;

void main()
{
    vec4 p0 = gl_TessCoord.x * controlPosition[0];
    vec4 p1 = gl_TessCoord.y * controlPosition[1];
    vec4 p2 = gl_TessCoord.z * controlPosition[2];
    patchDistance = gl_TessCoord;
    evaluationPosition = normalize(p0 + p1 + p2);
    gl_Position = evaluationPosition;
}
