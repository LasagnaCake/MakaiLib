#version 420 core

vec4 hueToPastel(float hue) {
	hue *= PI;
	vec3 res = vec3(
		cos(hue),
		cos(hue + TAU * (1.0/3.0)),
		cos(hue + TAU * (2.0/3.0))
	);
	res = normalize(res * res) * SQRT2;
	return vec4(clamp(res, vec3(0), vec3(1)), 1);
}

vec3 rgb2hsl(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsl2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float round(float v) {
	return floor(v + 0.5);
}

float ramp(float v) {
	return v - round(v);
}

float tri(float v) {
	return (abs(sin(v/2 + (HPI/2)))-abs(cos(v/2 + (HPI/2))));
}

float htri(float v) {
	return ramp(v/PI  + 2.5) * 2.0;
}

float hsin(float v) {
	return sin(ramp(v/PI  + 2.5) * PI);
}

float bin(float v, float lod) {
	return round(v * lod) / lod;
}