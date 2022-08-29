#version 410 core

in vec3 fragCoord3D;

void main(void) {
	gl_FragColor[0] = (fragCoord3D.x + 1) / 2;
	gl_FragColor[1] = (fragCoord3D.y + 1) / 2;
	gl_FragColor[2] = (fragCoord3D.z + 1) / 2;
	gl_FragColor[3] = 1.0;
}