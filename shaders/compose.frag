#version 120

in vec4 fragCoord3D;
in vec4 fragColor;

float lenV4(vec4 vec) {
	vec *= vec;
	return sqrt(vec.x + vec.y + vec.z + vec.w); 
}

void main() {
	fragColor.x = clamp(fragColor.x, 0, 1);
	fragColor.x = clamp(fragColor.y, 0, 1);
	fragColor.x = clamp(fragColor.z, 0, 1);
	fragColor.x = clamp(fragColor.w, 0, 1);
	gl_Color = fragColor;
}