#version 420

in vec2 fragUV;
in vec4 fragColor;

uniform sampler2D screen;
uniform sampler2D depth;

void main() {
	gl_FragColor = texture(screen, fragUV) * fragColor;
}