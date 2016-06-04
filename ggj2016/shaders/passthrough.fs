#version 330 core

uniform sampler2D texture;

in vec2 pass_position;
in vec2 pass_textureCoordinate;

void main()
{
    float t = (pass_position.x + 1.0) * 0.5;
    vec4 bricks_color = texture2D(bricks, pass_textureCoordinate);
    vec4 grass_color = texture2D(grass, pass_textureCoordinate);
    color = pass_color * mix(bricks_color, grass_color, t);
}