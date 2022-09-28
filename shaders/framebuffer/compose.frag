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
/**
* If -1, the average between the color channels is used.
* Else, the specified channel is used.
* MUST be between -1 and 3.
*/
uniform int maskChannel = 3;
uniform sampler2D mask;

uniform bool useChromatizer = false;
uniform bool dualChroma = false;
/**
* If not dual chroma, this means the channel to use for chromatizing.
* Else, this means the excluded channel.
* MUST be between 0 and 2.
*/
uniform uint chromaChannel = 0;

void main() {
	vec4 color = (texture(screen, fragUV) * fragColor * albedo) + accent;
	// Color inverter
	if (negative) color = vec4(vec3(1) - vec3(color.x, color.y, color.z), color.w);
	// Chromatizer (channel(s) as alpha)
	if (useChromatizer) {
		if (!dualChroma) {
			color.w = color[chromaChannel];
			switch (chromaChannel) {
			case 0:
				//color.w = (color.y + color.z) / 2;
				color.y = 0;
				color.z = 0;
				break;
			case 1:
				//color.w = (color.x + color.z) / 2;
				color.x = 0;
				color.z = 0;
				break;
			case 2:
				//color.w = (color.x + color.y) / 2;
				color.x = 0;
				color.y = 0;
				break;
			}
		} else {
			color.w = color[chromaChannel];
			color[chromaChannel] = 0;
		}
	}
	// Alpha mask
	if (useMask) {
		vec4 maskValue = texture(mask, maskUV);
		if (invertMask) maskValue = vec4(1) - maskValue;
		if (maskChannel < 0)
			color.w *= (maskValue.x + maskValue.y + maskValue.z) / 3;
		else
			color.w *= maskValue[maskChannel];
	}
	gl_FragColor = color;
}