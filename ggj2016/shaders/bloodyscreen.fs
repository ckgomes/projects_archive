#version 330 core

uniform sampler2D texture;

uniform vec3 screenColor;

in vec2 uv;

out vec4 color;

void main()
{

	color = vec4(screenColor, 1.0) + texture2D(texture, uv);

}