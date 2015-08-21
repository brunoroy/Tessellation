#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 delta;

out vec4 vertexPosition;
out vec4 vertexColor;

vec4 vertexTransform;

uniform mat4 mvp;
uniform bool doTessellation;
uniform bool doDisplacement;
uniform vec4 color;

void main()
{
    vertexTransform = vec4(position, 1.0f);

    //apply displacement if there is
    if (doDisplacement)
        vertexTransform += vec4(delta, 1.0f);

    //apply tessellation if activated
    if (doTessellation)
        vertexPosition = mvp * vertexTransform;
    else
        gl_Position = mvp * vertexTransform;

    vertexColor = color;
}
