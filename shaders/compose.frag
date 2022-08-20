#version 410

in vec3 fragCoord3D;

void main() {
	gl_FragColor = clamp(gl_FragColor, 0, 1);
}