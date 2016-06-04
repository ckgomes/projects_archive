#version 330 core

in vec2 position;

out vec2 uv;

uniform mat4 projection;
uniform vec2 camera;

void main()
{

	uv = (position / 30.0) - vec2(0.5);

	gl_Position = vec4(position - camera, 0.0, 1.0) * projection;

}