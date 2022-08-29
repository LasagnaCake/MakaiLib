#version 410

layout (location = 1) in vec2 vertUV;
uniform sampler2D screen;

void main() {
	gl_FragColor = clamp(vec4(texture(screen, vertUV)), 0, 1);
}