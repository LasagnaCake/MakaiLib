#version 420 core

#pragma optimize(on)

precision mediump float;

in vec3 fragCoord3D;
in vec2 fragUV;
in vec4 fragColor;
in vec3 fragNormal;

in vec2 warpUV;

in vec3 fragLightColor;
in vec3 fragShadeColor;

layout (location = 0) out vec4	FragColor;
//layout (location = 1) out float	DepthValue;

uniform bool		textured = false;
uniform sampler2D	texture2D;
uniform float		alphaClip = 0.2;

uniform bool		useEmission			= false;
uniform sampler2D	emissionTexture;
uniform float		emissionStrength	= 1.0;

uniform vec4 albedo = vec4(1);

// [ DISTANCE-BASED FOG ]
uniform bool	useFog		= false;
uniform float	fogFar		= 16;
uniform float	fogNear		= 12;
uniform float	fogStrength	= 1;
uniform vec4	fogColor	= vec4(1);

// [ VOID (NEAR FOG) ]
uniform bool	useVoid			= false;
uniform float	voidFar			= 6;
uniform float	voidNear		= 4;
uniform float	voidStrength	= 1;
uniform vec4	voidColor		= vec4(0);

// [ TEXTURE WARPING ]
uniform bool		useWarp			= false;
uniform sampler2D	warpTexture;
uniform uint		warpChannelX	=	0;
uniform uint		warpChannelY	=	1;

// [ COLOR INVERSION ]
uniform bool	useNegative			= false;
uniform float	negativeStrength	= 1;

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

// [ POINT LIGHTING ]
uniform bool	useLights	= false;

// [ HSLBC MODIFIERS ]
uniform float	hue			= 0;
uniform float	saturation	= 1;
uniform float	luminosity	= 1;
uniform float	brightness	= 0;
uniform float	contrast	= 1;

// [ DEBUG MODE ]
uniform uint	debugView	= 0;

vec4 distanceGradient(vec4 start, vec4 end, float near, float far, float strength) {
	// The vector's length needs to be calculated here, otherwise it breaks
	float value = (length(fragCoord3D) - near) / (far - near);
	//float fogValue = 1.0 - exp(-fragDistance * fogStrength);
	value = clamp(value * strength, 0, 1);
	return mix(start, end, value);
}

vec4 applyFog(vec4 color) {
	return distanceGradient(color, vec4(fogColor.rgb, fogColor.a * color.a), fogNear, fogFar, fogStrength);
}

vec4 applyVoid(vec4 color) {
	return distanceGradient(vec4(voidColor.rgb, fogColor.a * color.a), color, voidNear, voidFar, voidStrength);
}

vec4 applyGradient(vec4 color) {
	float gradientValue;
	if (gradientChannel < 0)
		gradientValue = (color.r + color.g + color.b) / 3.0;
	else
		gradientValue = color[gradientChannel];
	gradientValue = clamp(gradientValue, 0, 1);
	if (gradientInvert)
		return mix(gradientEnd, gradientStart, gradientValue);
	else
		return mix(gradientStart, gradientEnd, gradientValue);
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

vec4 applyHSL(vec4 color) {
	vec3 hsl = rgb2hsl(color.xyz);
	hsl.x += mod(hue, 1);
    hsl.yz *= vec2(saturation, luminosity);
	return vec4(hsl2rgb(hsl), color.a);
}

vec4 applyBrightnessAndContrast(vec4 color) {
	vec3 res = color.rgb + brightness.xxx;
	return vec4(((res - 0.5f) * max(contrast, 0)) + 0.5f, color.a);
}

void main(void) {
	vec4 color;
	vec2 calculatedFragUV = fragUV;
	if (textured) {
		if (useWarp) {
			uint wcx = clamp(warpChannelX, 0u, 3u);
			uint wcy = clamp(warpChannelY, 0u, 3u);
			vec4 warpFac = texture(warpTexture, warpUV);
			vec2 warpCoord = vec2(warpFac[wcx], warpFac[wcy]) * 2 - 1;
			calculatedFragUV = fragUV + warpCoord;
			color = texture(texture2D, calculatedFragUV) * fragColor;
		}
		else color = texture(texture2D, fragUV) * fragColor;
	}
	else color = fragColor;

	if (textured && color.a <= (fragColor.a * alphaClip))
		discard;

	color.xyz *= fragLightColor * fragShadeColor;

	color *= albedo;

	color = applyHSL(color);

	color = applyBrightnessAndContrast(color);

	if (useNegative) {
		vec4 nc = vec4(vec3(1) - color.rgb, color.a);
		color = mix(color, nc, negativeStrength);
	}

	if (useGradient) color = applyGradient(color);

	if (useVoid) color = applyVoid(color);

	if (useFog) color = applyFog(color);

	if (useEmission) {
		vec4	emitColor	= texture(emissionTexture, calculatedFragUV) * fragColor;
		float	emitFactor	= rgb2hsl(emitColor.rgb).z * emitColor.a;
		color.rgb = mix(color.rgb, emitColor.rgb, emitFactor * emissionStrength);
	}

	FragColor = color;

	if (debugView > 0) {
		switch(debugView) {
			case 1: FragColor = vec4(fragNormal / 2 + 0.5, 1); break;
			default: break;
		}
	}

	//gl_FragDepth = length(fragCoord3D);
	//DepthValue = length(fragCoord3D);

	//FragColor = vec4(fragColor.x, 1, 1.1-(fragDistance/50.0), 1) * albedo;
}