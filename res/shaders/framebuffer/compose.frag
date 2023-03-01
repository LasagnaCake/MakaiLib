#version 420 core

in vec2 fragUV;
in vec2 warpUV;
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
uniform float waveLOD		= 4;

// [ SCREEN PRISM ]
uniform bool usePrism		= false;
uniform vec2 prismAmplitude	= vec2(1);
uniform vec2 prismFrequency	= vec2(1);
uniform vec2 prismShift		= vec2(1);
uniform uint prismShape		= 0;
uniform float prismLOD		= 4;

// [ TEXTURE WARPING ]
uniform bool		useWarp			= false;
uniform sampler2D	warpTexture;
uniform uint		warpChannelX	=	0;
uniform uint		warpChannelY	=	1;

// [ SCREEN RAINBOW EFFECT ]
uniform bool	useRainbow			= false;
uniform vec2	rainbowFrequency	= vec2(0);
uniform vec2	rainbowShift		= vec2(0);
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

// [ HSL MODIFIERS ]
uniform float	hue			= 0;
uniform float	saturation	= 1;
uniform float	luminosity	= 1;

// [ DEBUG SETTINGS ]
uniform bool	useDebug	= false;
uniform uint	debugView	= 0;

#ifndef PI
#define PI 3.1415926535
#endif

#ifndef HPI
#define HPI (PI / 2.0)
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

vec3 rgb2hsl(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsl2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
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
	vec2 rv = coords * rainbowFrequency + rainbowShift;
	vec4 rainbow = vec4(hsl2rgb(vec3(rv.x + rv.y, 1, 1)), 1);
	rainbow = mix(vec4(1), rainbow, rainbowStrength);
	if (rainbowAbsolute)
		rainbow.xyz *= color.xyz;
	return vec4(rainbow.xyz, color.w);
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

float ramp(float v) {
	return v - round(v);
}

float tri(float v) {
	return (abs(sin(v/2 + (HPI/2)))-abs(cos(v/2 + (HPI/2))));
}

float htri(float v) {
	return ramp(v/PI  + 2.5) * 2.0;
}

float hsin(float v) {
	return sin(ramp(v/PI  + 2.5) * PI);
}

float bin(float v, float lod) {
	return round(v * lod) / lod;
}

float pattern(float t, uint shape, float lod) {
	switch (shape) {
		// Square wave
		default:
		case 0:	return sign(sin(t));
		// Sine wave
		case 1:	return sin(t);
		case 2: return bin(sin(t), lod);
		// Triangle wave
		case 3: return tri(t);
		case 4: return bin(tri(t), lod);
		// Half-sine wave
		case 5: return hsin(t);
		case 6: return bin(hsin(t), lod);
		// Half-triangle wave
		case 7: return htri(t);
		case 8: return bin(htri(t), lod);
	}
}

vec2 patternV2(vec2 t, uint shape, float lod) {
	return vec2(pattern(t.x, shape, lod), pattern(t.y, shape, lod));
}

vec4 applyOutline(vec4 color, vec2 uv) {
	return mix(color, outlineColor, getOutlineValue(uv, outlineSize) - color.a);
}

vec4 applyHSL(vec4 color) {
	vec3 hsl = rgb2hsl(color.xyz);
	hsl.x += mod(hue, 1);
    hsl.yz *= vec2(saturation, luminosity);
	return vec4(hsl2rgb(hsl), color.a);
}

void main() {
	// Screen wave	
	vec2 wave = vec2(0);
	if (useWave) {
		wave = (fragUV.yx * waveFrequency) * (2.0 * PI) + waveShift;
		wave = patternV2(wave, waveShape, waveLOD) * (waveAmplitude / 10.0);
	}

	// Screen prismatic effect
	vec2 prism = vec2(0);
	if (usePrism) {
		prism = (fragUV * prismFrequency.yx) * (2.0 * PI) + prismShift.yx;
		prism = patternV2(prism, prismShape, prismLOD) * (prismAmplitude.yx / 10.0);
	}

	// Screen texture warping
	vec2 warp = vec2(0);
	if (useWarp) {
		vec4 warpFac = texture(warpTexture, warpUV);
		warp = vec2(warpFac[warpChannelX], warpFac[warpChannelY]) * 2 - 1;
	}

	// Get pixel color
	vec2 screenUV = fragUV + prism + wave + warp;
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

	FragColor = applyHSL(color);

	if (useDebug) {
		switch(debugView) {
			case 1: FragColor = texture(depth, fragUV); break;
			default: break;
		}
	}
}