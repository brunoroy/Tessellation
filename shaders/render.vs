#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 mvp;
uniform mat3 normalMatrix;
uniform mat3 viewMatrix;
uniform vec3 lightDirection;

out vec2 fragmentUV;
out vec4 fragmentNormal;
out vec4 fragmentLightDirection;

void main()
{
    vec4 initialPosition = vec4(position, 1.0f);
    gl_Position = mvp * initialPosition;
    fragmentUV = uv;
    fragmentNormal = vec4(normalMatrix * normal, 1.0f);
    fragmentLightDirection = vec4(viewMatrix * lightDirection, 1.0f);
    initialPosition = gl_Position;
}
