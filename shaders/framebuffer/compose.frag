#version 420

in vec2 fragUV;
in vec4 fragColor;

uniform vec4 albedo = vec4(1);

uniform sampler2D screen;
uniform sampler2D depth;

void main() {
	vec4 color = texture(screen, fragUV) * fragColor * albedo;
	gl_FragColor = color;
}