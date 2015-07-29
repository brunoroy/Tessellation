#version 400

layout(triangles, equal_spacing, cw) in;
in vec3 controlPosition[];
out vec3 evaluationPosition;
out vec3 patchDistance;
//uniform mat4 projection;
//uniform mat4 Modelview;

void main()
{
    vec3 p0 = gl_TessCoord.x * controlPosition[0];
    vec3 p1 = gl_TessCoord.y * controlPosition[1];
    vec3 p2 = gl_TessCoord.z * controlPosition[2];
    patchDistance = gl_TessCoord;
    evaluationPosition = normalize(p0 + p1 + p2);
    gl_Position = vec4(evaluationPosition, 1.0);//Projection * Modelview * vec4(tePosition, 1);
}
