#version 330 core

in vec2 position;

in vec2 instancePosition;
in vec4 instanceColor;

out vec2 fragmentPosition;
out vec2 lightCenter;
out vec4 lightColor;

uniform mat4 projection;

void main()
{

    vec2 transformedPosition = position + instancePosition;

    fragmentPosition = transformedPosition;
    lightCenter = instancePosition;
    lightColor = instanceColor;

    gl_Position = vec4(transformedPosition, 0.0, 1.0) * projection;

}