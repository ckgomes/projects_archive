#version 330 core

uniform sampler2D sceneFramebuffer;

uniform vec4 ambientLight;

in vec2 uv;

out vec4 color;

void main()
{
    color = ambientLight * texture2D(sceneFramebuffer, uv);
}