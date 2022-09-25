#version 420

in vec2 fragUV;
in vec4 fragColor;

uniform vec4 albedo = vec4(1);
uniform vec4 accent = vec4(0);

uniform sampler2D screen;
uniform sampler2D depth;

uniform bool useAlphaMask = false;
uniform sampler2D alphaMask;

void main() {
	vec4 color = (texture(screen, fragUV) * fragColor * albedo) + vec4(accent.x, accent.y, accent.z, accent.w);
	if (useAlphaMask) color.w *= texture(alphaMask, fragUV).a;
	gl_FragColor = color;
}