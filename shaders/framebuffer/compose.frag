#version 410

in vec2 fragUV;
uniform sampler2D screen;

void main() {
	gl_FragColor = clamp(vec4(texture(screen, fragUV)), 0, 1);
}