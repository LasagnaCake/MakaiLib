#version 420 core

precision mediump float;

in vec2 fragUV;
in vec2 warpUV;
in vec2 maskUV;
in vec4 fragColor;
in vec2 screenScale;

layout (location = 0) out vec4	FragColor;

uniform vec2 resolution = vec2(0);

uniform vec4 albedo = vec4(1);
uniform vec4 accent = vec4(0);

uniform sampler2D screen;
uniform sampler2D depth;

// [ COLOR INVERSION ]
uniform bool	useNegative			= false;
uniform float	negativeStrength	= 1;

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

// [ SCREEN POLAR DISTORTION ]
uniform bool	usePolarWarp			= true;
uniform vec2	polarWarpPosition		= vec2(800, 450);
uniform vec2	polarWarpStrength		= vec2(1);
uniform float	polarWarpSize			= 200;
uniform vec4	polarWarpColor			= vec4(0,0,0.25,1);
uniform float	polarWarpTintStrength	= 1;
uniform bool	polarWarpFishEye		= true;

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

// [ NOISE EFFECT ]
uniform bool	useNoise				= false;
uniform float	noiseStrength			= 1;
uniform float	noiseSeed				= 1;
uniform uint	noiseType				= 0;
uniform bool	noiseAbsolute			= true;

// [ HSL MODIFIERS ]
uniform float	hue			= 0;
uniform float	saturation	= 1;
uniform float	luminosity	= 1;

// [ DEBUG SETTINGS ]
uniform uint	debugView	= 0;

#ifndef PI
#define PI 3.1415926535
#endif

#ifndef HPI
#define HPI (PI / 2.0)
#endif

#ifndef PHI
#define PHI 1.61803398874989484820459
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

vec3 rgb2hsl(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsl2rgb(vec3 c) {
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

float angleTo(vec2 vec) {
	return - atan(vec.y, vec.x) + HPI;
}

vec2 angleV2(float angle) {
	return vec2(sin(angle), cos(angle));
}

vec2 normalTo(vec2 vec) {
	return angleV2(angleTo(vec));
}

vec2 polarWarp(out float pfac) {
	vec2 target = (polarWarpPosition - gl_FragCoord.xy);
	float distance =  clamp(1 - length(target) / polarWarpSize, 0, 1);
	pfac = distance;
	if (polarWarpFishEye)
		distance = sin(distance * PI);
	vec2 offset = normalTo(target);
	return (offset * distance) * (polarWarpStrength / 100);
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
		case 0x00:	return sign(sin(t));
		// Sine wave
		case 0x01:	return sin(t);
		case 0x02:	return bin(sin(t), lod);
		// Triangle wave
		case 0x03:	return tri(t);
		case 0x04:	return bin(tri(t), lod);
		// Half-sine wave
		case 0x05:	return hsin(t);
		case 0x06:	return bin(hsin(t), lod);
		// Half-triangle wave
		case 0x07:	return htri(t);
		case 0x08:	return bin(htri(t), lod);
		// Noise
		case 0x09:	return noise1(t);
		case 0x0A:	return bin(noise1(t), lod);
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

float simplenoise(vec2 xy) {
	return fract(sin(dot(xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float goldnoise(vec2 xy, float seed) {
	return fract(tan(distance(xy*PHI, xy)*seed));
}

float supernoise(vec2 xy, float seed) {
	return goldnoise(vec2(simplenoise(xy), simplenoise(xy.yx)), seed);
}

float rand(vec2 xy, uint type, float seed){
    switch (type) {
    	default:
    	case 0x00:	return supernoise(xy, seed);
    	case 0x01:	return simplenoise(xy);
    	case 0x02:	return goldnoise(xy, seed);
    }
}

vec4 applyNoise(vec4 color, vec2 uv) {
	vec2 nc = uv + color.xy + color.zw;
	float nv = rand(nc, noiseType, noiseSeed);
	vec4 res = noiseAbsolute ? vec4(nv, nv, nv, 1) : vec4(color.xyz, nv);
	return mix(color, res, noiseStrength);
}

void main() {
	// Screen wave	
	vec2 wave = vec2(0);
	if (useWave) {
		wave = (fragUV.yx * waveFrequency) * TAU + waveShift;
		wave = patternV2(wave, waveShape, waveLOD) * (waveAmplitude / 10.0);
	}

	// Screen prismatic effect
	vec2 prism = vec2(0);
	if (usePrism) {
		prism = (fragUV * prismFrequency.yx) * TAU + prismShift.yx;
		prism = patternV2(prism, prismShape, prismLOD) * (prismAmplitude.yx / 10.0);
	}

	// Screen polar distortion
	vec2 pwarp = vec2(0);
	float pfac = 0;
	if (usePolarWarp) {
		pwarp = polarWarp(pfac);
	}

	// Screen texture warping
	vec2 warp = vec2(0);
	if (useWarp) {
		vec4 warpFac = texture(warpTexture, warpUV);
		warp = vec2(warpFac[warpChannelX], warpFac[warpChannelY]) * 2 - 1;
	}

	// Get pixel color
	vec2 screenUV = fragUV + prism + wave + warp + pwarp;
	vec4 color = (getPixelColor(screenUV) * fragColor * albedo) + accent;

	// Color inverter
	if (useNegative) {
		vec4 nc = vec4(vec3(1) - color.xyz, color.w);
		color = mix(color, nc, negativeStrength);
	}

	// Color to gradient
	if (useGradient) color = applyGradient(color);

	// Rainbow effect
	if (useRainbow) color = applyRainbow(color, screenUV);

	// Outline effect
	if (useOutline) color = applyOutline(color, screenUV);

	// Noise effect
	if (useNoise) color = applyNoise(color, screenUV);

	// Alpha mask
	if (useMask) {
		vec4 maskValue = texture(mask, maskUV);
		if (invertMask) maskValue = vec4(1) - maskValue;
		color *= maskValue;
	}

	if (color.w <= 0) discard;

	color = applyHSL(color);

	FragColor = mix(color, polarWarpColor, pfac * polarWarpTintStrength);

	if (debugView > 0) {
		switch(debugView) {
			case 1: FragColor = texture(depth, fragUV); break;
			default: break;
		}
	}
}