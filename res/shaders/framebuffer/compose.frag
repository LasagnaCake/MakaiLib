#version 420 core

in vec2 fragUV;
in vec2 maskUV;
in vec4 fragColor;

layout (location = 0) out vec4 FragColor;

uniform vec4 albedo = vec4(1);
uniform vec4 accent = vec4(0);

uniform sampler2D screen;
uniform sampler2D depth;

// [ COLOR INVERSION ]

uniform bool negative = false;

// [ ALPHA MASK ]

uniform bool useMask = false;
uniform bool invertMask = false;
/**
* If -1, the average between the color channels is used.
* Else, the specified channel is used.
* MUST be between -1 and 3.
*/
uniform int			maskChannel = 3;
uniform sampler2D	mask;

// [ CHROMATIZER ]

uniform bool	useChromatizer = false;
uniform vec4	chromaColor = vec4(1,0,0,1);
/**
* If -1, the average between the color channels is used.
* Else, the specified channel is used.
* MUST be between -1 and 3.
*/
uniform int		chromaChannel = 0;

// [ HSL ADJUSTMENTS ]

uniform float saturation = 1;

// [ SCREEN WAVE ]
uniform bool useWave		= false;
uniform vec2 waveAmplitude	= vec2(1);
uniform vec2 waveFrequency	= vec2(1);
uniform vec2 waveShift		= vec2(1);

#ifndef PI
#define PI 3.1415926535
#endif

void main() {
	// Screen wave	
	vec2 wave = vec2(0);
	if (useWave) {
		wave = ((fragUV + waveShift) * waveFrequency) * (2.0 * PI);
		wave = vec2(sin(wave.x), sin(wave.y)) * waveAmplitude;
	}
	vec4 color = (texture(screen, fragUV) * fragColor * albedo) + accent;
	// HSL adjuster
	color = mix(vec4(vec3(color.x, color.y, color.z), color.w), color, saturation);
	// Color inverter
	if (negative) color = vec4(vec3(1) - vec3(color.x, color.y, color.z), color.w);
	// Chromatizer (channel(s) as alpha)
	if (useChromatizer) {
		vec4 chromaValue = chromaColor;
		if (chromaChannel < 0)
			chromaValue.w = clamp((color.x + color.y + color.z) / 3, 0, 1);
		else
			chromaValue.w = clamp(color[chromaChannel], 0, 1);
		color = chromaValue;
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

	if (color.w > 0)
		color = color / vec4(vec3(color.w), 1.0);
	FragColor = color;
}