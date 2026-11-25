#version 330 core

in vec3 vertexColor;

out vec4 FragColor;

uniform vec3 objectColor;
uniform bool useVertexColor;

void main()
{
    if (useVertexColor)
    {
        FragColor = vec4(vertexColor, 1.0);
    }
    else
    {
        FragColor = vec4(objectColor, 1.0);
    }
}