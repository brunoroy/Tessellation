#version 400

layout (location = 0) out vec4 color;

in vec4 vertexColor;

void main()
{
    color = vertexColor;
}
