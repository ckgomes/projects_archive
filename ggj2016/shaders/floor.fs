#version 330 core

uniform sampler2D texture;

in vec2 uv;

out vec4 color;

float dithering()
{
	return (sin(uv.x * 0.2) + 1.0) * (cos(uv.y * 0.2) + 1.0) * 0.25 * 0.5;
}

void main()
{

	vec4 s = texture2D(texture, uv);
	color = s - dithering() / (2.0 * s.r + 1.0);

}