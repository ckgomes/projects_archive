#version 330 core

in vec2 position;

out vec2 uv;

uniform vec2 translation;

void main()
{
	uv = step(vec2(0.0), position);
	uv.y = 1.0f - uv.y;
	gl_Position = vec4(position + translation, 0.0, 1.0);
}