#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out vec4 vertexPosition;

uniform mat4 mvp;
uniform bool doTessellation;

void main()
{
    if (doTessellation)
        vertexPosition = mvp * vec4(position, 1.0f);
    else
        gl_Position = mvp * vec4(position, 1.0f);
}
