#version 330 core

in vec2 position;

uniform mat3 transform;
uniform mat4 projection;

void main()
{
    vec3 transformed_position = vec3(position, 1.0) * transform;
    gl_Position = vec4(transformed_position, 1.0) * projection;
}