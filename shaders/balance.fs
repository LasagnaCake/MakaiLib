#version 120

inout vec4 fragColor;

float lenV4(vec4 vec) {
	vec *= vec;
	return sqrt(vec.x + vec.y + vec.z + vec.w); 
}

void main() {
	fragColor /= lenV4(fragColor);
}