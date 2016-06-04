#version 330 core

uniform sampler2D texture;

in vec2 uv;

out vec4 color;

void main()
{
    color = texture2D(texture, uv);
    if (color.w == 0.0) discard;
}