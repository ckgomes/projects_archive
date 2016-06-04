#version 330 core

uniform float time;
uniform vec2 resolution;

out vec4 color;

float rand(vec2 n)
{ 
	return fract(cos(dot(n, vec2(5.14229, 433.494437))) * 2971.215073);
}

float noise(vec2 n)
{
	const vec2 d = vec2(0.0, 1.0);
	vec2 b = floor(n);
	vec2 f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float fbm(vec2 n)
{
	float total = 0.0;
	float amplitude = 1.0;
	for (int i = 0; i < 6; i++)
	{
		total += noise(n) * amplitude;
		amplitude *= 0.6;
		n += n;
	}
	return total;
}

void main() {
	
	const vec3 c1 = vec3(0.2, 0.3, 0.1);
	const vec3 c2 = vec3(0.9, 0.1, 0.0);
	const vec3 c3 = vec3(0.2, 0.0, 0.0);
	const vec3 c4 = vec3(1.0, 0.9, 0.0);
	const vec3 c5 = vec3(0.1);
	const vec3 c6 = vec3(0.9);
	
	vec2 p = gl_FragCoord.xy * 8.0 / resolution.xx;
	float q = fbm(p - time * 0.25);
	vec2 r = vec2(fbm(p + q + log2(time * 0.7) - p.x - p.y), fbm(p + q - abs(log2(time * 3.14))));
	vec3 c = mix(c1, c2, fbm(p + r)) + mix(c3, c4, r.x) - mix(c5, c6, r.y);
	color = vec4(c *  cos(gl_FragCoord.y / resolution.y), 1.0);
}
