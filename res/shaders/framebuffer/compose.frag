#version 420 core

in vec2 fragUV;
in vec2 maskUV;
in vec4 fragColor;
in vec2 screenScale;

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
uniform uint waveShape		= 0;

// [ SCREEN WAVE ]
uniform bool usePrism		= false;
uniform vec2 prismAmplitude	= vec2(1);
uniform vec2 prismFrequency	= vec2(1);
uniform vec2 prismShift		= vec2(1);
uniform uint prismShape		= 0;

// [ SCREEN RAINBOW EFFECT ]
uniform bool	useRainbow			= false;
uniform float	rainbowFrequency	= 0;
uniform float	rainbowShift		= 0;
uniform float	rainbowStrength		= 0;
uniform bool	rainbowAbsolute		= false;

// [ BLURRING EFFECT ]
uniform bool	useBlur			= false;
uniform vec2	blurStrength	= vec2(0.0005);

// [ OUTLINE EFFECT ]

uniform bool	useOutline				= false;
uniform vec2	outlineSize				= vec2(0.01);
uniform vec4	outlineColor			= vec4(1);
uniform bool	outlineRelativeAlpha	= true;

#ifndef PI
#define PI 3.1415926535
#endif

#ifndef TAU
#define TAU (PI * 2.0)
#endif

#ifndef SQRT2
#define SQRT2 1.4142135623
#endif

// TODO?: www.youtube.com/watch?v=5EuYKEvugLU

vec4 hueToPastel(float hue) {
	hue *= PI;
	vec3 res = vec3(
		cos(hue),
		cos(hue + TAU * (1.0/3.0)),
		cos(hue + TAU * (2.0/3.0))
	);
	res = normalize(res * res) * SQRT2;
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
		return rainbow * vec4(vec3(1), color.w);
	else
		return color * rainbow;
}

vec4 getPixelColor(vec2 uv) {
	if (!useBlur) return texture(screen, uv);
	return (
		texture(screen, uv) +
		texture(screen, uv + vec2(blurStrength.x, 0)) +
		texture(screen, uv - vec2(blurStrength.x, 0)) +
		texture(screen, uv + vec2(0, blurStrength.y)) +
		texture(screen, uv - vec2(0, blurStrength.y))
	) / 5.0;
}

float getOutlineValue(vec2 uv, vec2 oSize) {
	vec2 size = oSize / screenScale;
	float outline =
		texture(screen, uv + vec2(-size.x, 0)).a
	+	texture(screen, uv + vec2(0, size.y)).a
	+	texture(screen, uv + vec2(size.x, 0)).a
	+	texture(screen, uv + vec2(0, -size.y)).a
	+	texture(screen, uv + vec2(-size.x, size.y)).a
	+	texture(screen, uv + vec2(size.x, size.y)).a
	+	texture(screen, uv + vec2(-size.x, -size.y)).a
	+	texture(screen, uv + vec2(size.x, -size.y)).a;
	if (outlineRelativeAlpha) return min(outline, 1.0);
	else return (outline > 0.0) ? 1.0 : 0.0;
}

float round(float v) {
	return floor(v + 0.5);
}

float pattern(float t, uint shape) {
	switch (shape) {
		default:
		case 0:	return sin(t);
		case 1:	return sign(sin(t));
		case 2: return (abs(sin(t))-abs(cos(t))) / 2;
		case 3: return round(sin(t) * 4.0) / 4.0;
		case 4: return t - ceil(t);
	}
}

vec2 patternV2(vec2 t, uint shape) {
	return vec2(pattern(t.x, shape), pattern(t.y, shape));
}

vec4 applyOutline(vec4 color, vec2 uv) {
	return mix(color, outlineColor, getOutlineValue(uv, outlineSize) - color.a);
}

void main() {
	// Screen wave	
	vec2 wave = vec2(0);
	if (useWave) {
		wave = (fragUV.yx * waveFrequency) * (2.0 * PI) + waveShift;
		wave = patternV2(wave, waveShape) * (waveAmplitude / 10.0);
	}
	// Screen plasmicity
	vec2 prism = vec2(0);
	if (usePrism) {
		prism = (fragUV * prismFrequency) * (2.0 * PI) + prismShift;
		prism = patternV2(prism, prismShape) * (prismAmplitude / 10.0);
	}
	vec2 screenUV = fragUV + wave + prism;
	vec4 color = (getPixelColor(screenUV) * fragColor * albedo) + accent;
	// Color inverter
	if (negative) color = vec4(vec3(1) - vec3(color.x, color.y, color.z), color.w);
	// Color to gradient
	if (useGradient) color = applyGradient(color);
	// Rainbow effect
	if (useRainbow) color = applyRainbow(color, screenUV);
	// Outline effect
	if (useOutline) color = applyOutline(color, screenUV);
	// Alpha mask
	if (useMask) {
		vec4 maskValue = texture(mask, maskUV);
		if (invertMask) maskValue = vec4(1) - maskValue;
		color *= maskValue;
	}
	if (color.w <= 0) discard;
	FragColor = color;
}