#version 400

//uniform mat4 Modelview;
//uniform mat3 NormalMatrix;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in vec3 evaluationPosition[3];
in vec3 patchDistance[3];
out vec3 faceNormal;
out vec3 distance;
out vec3 triangleDistance;

void main()
{
    vec3 A = evaluationPosition[2] - evaluationPosition[0];
    vec3 B = evaluationPosition[1] - evaluationPosition[0];
    //gFacetNormal = NormalMatrix * normalize(cross(A, B));

    distance = patchDistance[0];
    triangleDistance = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position; EmitVertex();

    distance = patchDistance[1];
    triangleDistance = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position; EmitVertex();

    distance = patchDistance[2];
    triangleDistance = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position; EmitVertex();

    EndPrimitive();
}
