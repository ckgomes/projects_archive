#version 330 core

in vec2 position;

out vec2 uv;

uniform vec2 camera;

uniform vec2 translation;
uniform float rotation;
uniform float scale;

uniform mat4 projection;

uniform vec2 min_uv;
uniform vec2 max_uv;

vec2 transform()
{
	float sine = sin(rotation) * scale;
	float cosine = cos(rotation) * scale;
	return  vec2(cosine * position.x - sine * position.y + translation.x - camera.x, sine * position.x + cosine * position.y + translation.y - camera.y);
}

void main()
{
    uv = mix(min_uv, max_uv, (position + vec2(1.0)) * 0.5);
    gl_Position = vec4(transform(), 0.0, 1.0) * projection;
}