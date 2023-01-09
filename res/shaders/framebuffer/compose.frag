#version 420 core

in vec2 fragUV;
in vec2 maskUV;
in vec4 fragColor;

layout (location = 0) out vec4	FragColor;

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

// [ COLOR TO GRADIENT ]
uniform bool	useGradient		= false;
/**
* If -1, the average between the color channels is used.
* Else, the specified channel is used.
* MUST be between -1 and 3.
*/
uniform int		gradientChannel	= -1;
uniform vec4	gradientStart	= vec4(0);
uniform vec4	gradientEnd		= vec4(1);
uniform bool	gradientInvert	= false;

// [ SCREEN WAVE ]
uniform bool useWave		= false;
uniform vec2 waveAmplitude	= vec2(1);
uniform vec2 waveFrequency	= vec2(1);
uniform vec2 waveShift		= vec2(1);

// [ SCREEN RAINBOW EFFECT ]
uniform bool	useRainbow			= false;
uniform float	rainbowFrequency	= 0;
uniform float	rainbowShift		= 0;
uniform float	rainbowStrength		= 0;
uniform bool	rainbowAbsolute		= false;

#ifndef PI
#define PI 3.1415926535
#endif

#ifndef TAU
#define TAU (PI * 2.0)
#endif

// TODO?: www.youtube.com/watch?v=5EuYKEvugLU

vec4 hueToPastel(float hue) {
	hue *= PI;
	vec3 res = vec3(
		cos(hue),
		cos(hue + TAU * (1.0/3.0)),
		cos(hue + TAU * (2.0/3.0))
	);
	res = normalize(res * res) * sqrt(2);
	return vec4(clamp(res, vec3(0), vec3(1)), 1);
}

vec4 applyGradient(vec4 color) {
	float gradientValue;
	if (gradientChannel < 0)
		gradientValue = (color.x + color.y + color.z) / 3.0;
	else
		gradientValue = color[gradientChannel];
	gradientValue = clamp(gradientValue, 0, 1);
	if (gradientInvert)
		return mix(gradientEnd, gradientStart, gradientValue);
	else
		return mix(gradientStart, gradientEnd, gradientValue);
}

vec4 applyRainbow(vec4 color, vec2 coords) {
	vec4 rainbow = hueToPastel((coords.x + coords.y) * rainbowFrequency + rainbowShift);
	rainbow = mix(rainbow, vec4(1), rainbowStrength);
	if (rainbowAbsolute)
		return rainbow;
	else
		return color * rainbow;
}

void main() {
	// Screen wave	
	vec2 wave = vec2(0);
	if (useWave) {
		wave = (fragUV.yx * waveFrequency) * (2.0 * PI) + waveShift;
		wave = vec2(sin(wave.x), sin(wave.y)) * (waveAmplitude / 10.0);
	}
	vec4 color = (texture(screen, fragUV + wave) * fragColor * albedo) + accent;
	if (color.w <= 0) discard;
	// Color inverter
	if (negative) color = vec4(vec3(1) - vec3(color.x, color.y, color.z), color.w);
	// Color to gradient
	if (useGradient) color = applyGradient(color);
	// Reinbow effect
	if (useRainbow) color = applyRainbow(color, fragUV + wave);
	// Alpha mask
	if (useMask) {
		vec4 maskValue = texture(mask, maskUV);
		if (invertMask) maskValue = vec4(1) - maskValue;
		if (maskChannel < 0)
			color.w *= (maskValue.x + maskValue.y + maskValue.z) / 3;
		else
			color *= maskValue;
	}
	FragColor = color;
}