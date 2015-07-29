#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out vec3 vertexPosition;

uniform mat4 mvp;

void main()
{
    //vertexPosition = position;
    vertexPosition = vec3(mvp * vec4(position, 1.0f));
}
