#version 420

in vec2 fragUV;
in vec2 maskUV;
in vec4 fragColor;

uniform vec4 albedo = vec4(1);
uniform vec4 accent = vec4(0);

uniform sampler2D screen;
uniform sampler2D depth;

uniform bool negative = false;

uniform bool useMask = false;
uniform bool invertMask = false;
uniform int channel = 3;
uniform sampler2D mask;

void main() {
	vec4 color = (texture(screen, fragUV) * fragColor * albedo) + accent;
	if (negative) color = vec4(vec3(1) - vec3(color.x, color.y, color.z), color.w);
	if (useMask) {
		vec4 maskValue = texture(mask, maskUV);
		if (invertMask) maskValue = vec4(1) - maskValue;
		if (channel < 0)
			color.w *= (maskValue.x + maskValue.y + maskValue.z) / 3;
		else
			color.w *= maskValue[channel];
	}
	gl_FragColor = color;
}