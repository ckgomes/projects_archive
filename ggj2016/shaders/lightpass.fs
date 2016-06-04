#version 330 core

in vec2 fragmentPosition;
in vec2 lightCenter;
in vec4 lightColor;

out vec4 color;

const vec3 lightAttenuation = vec3(3.2, 2.6, 2.5);

void main()
{

    float d = distance(lightCenter, fragmentPosition);
    float a = 1.0 / ((d*d) * lightAttenuation.x + d * lightAttenuation.y + lightAttenuation.z);

    color = lightColor * a;

}