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

vec3 rgb2hsl(vec3 color) {
  float cmax = max(max(color.r, color.g), color.b);
  float cmin = min(min(color.r, color.g), color.b);
  float delta = cmax - cmin;
  
  float hue = 0.0;
  if (delta == 0.0) {
    hue = 0.0;
  } else if (cmax == color.r) {
    hue = mod((color.g - color.b) / delta, 6.0);
  } else if (cmax == color.g) {
    hue = (color.b - color.r) / delta + 2.0;
  } else {
    hue = (color.r - color.g) / delta + 4.0;
  }
  hue *= 60.0;
  if (hue < 0.0) {
    hue += 360.0;
  }
  
  float lightness = (cmax + cmin) / 2.0;
  
  float saturation = 0.0;
  if (delta == 0.0) {
    saturation = 0.0;
  } else {
    saturation = delta / (1.0 - abs(2.0 * lightness - 1.0));
  }
  
  return vec3(hue, saturation, lightness);
}

vec3 hsl2rgb(vec3 hsl) {
  float hue = hsl.x;
  float saturation = hsl.y;
  float lightness = hsl.z;
  
  float chroma = (1.0 - abs(2.0 * lightness - 1.0)) * saturation;
  float x = chroma * (1.0 - abs(mod(hue / 60.0, 2.0) - 1.0));
  float m = lightness - chroma / 2.0;
  
  vec3 color = vec3(0.0);
  if (hue < 60.0) {
    color = vec3(chroma, x, 0.0);
  } else if (hue < 120.0) {
    color = vec3(x, chroma, 0.0);
  } else if (hue < 180.0) {
    color = vec3(0.0, chroma, x);
  } else if (hue < 240.0) {
    color = vec3(0.0, x, chroma);
  } else if (hue < 300.0) {
    color = vec3(x, 0.0, chroma);
  } else {
    color = vec3(chroma, 0.0, x);
  }
  
  return color + vec3(m);
}

vec4 applyHSL(vec4 color) {
	return vec4(hsl2rgb(rgb2hsl(color.xyz) * vec3(1, saturation, luminosity)) + vec3(hue,0,0), color.a);
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