#version 420

in vec2 fragUV;
in vec4 fragColor;

uniform vec4 albedo = vec4(1);
uniform vec4 accent = vec4(0);

uniform sampler2D screen;
uniform sampler2D depth;

void main() {
	vec4 color = (texture(screen, fragUV) * fragColor * albedo) + vec4(accent.x, accent.y, accent.z, 0);
	gl_FragColor = color;
}