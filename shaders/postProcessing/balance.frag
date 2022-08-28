#version 410

void main() {
	float alpha = gl_FragColor[3];
	vec4 vec = gl_FragColor * gl_FragColor;
	float len = min(sqrt(vec.x + vec.y + vec.z + vec.w), 1);
	gl_FragColor /= vec4(len);
	gl_FragColor.w = alpha;
}