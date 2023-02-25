#version 420 core

in vec3 fragCoord3D;
in vec2 fragUV;
in vec4 fragColor;
in vec3 fragNormal;

in vec2 warpUV;

in vec3 fragLightColor;

layout (location = 0) out vec4	FragColor;
//layout (location = 1) out float	DepthValue;

uniform bool		textured = false;
uniform sampler2D	texture2D;
uniform float		alphaClip = 0.2;

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
uniform bool	useNegative	= false;

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

vec4 applyLights(vec4 color) {
	return vec4(color.xyz * fragLightColor, color.w);
}

vec4 distanceGradient(vec4 start, vec4 end, float near, float far, float strength) {
	// The vector's length needs to be calculated here, otherwise it breaks
	float value = (length(fragCoord3D) - near) / (far - near);
	//float fogValue = 1.0 - exp(-fragDistance * fogStrength);
	value = clamp(value * strength, 0, 1);
	return mix(start, end, value);
}

vec4 applyFog(vec4 color) {
	return distanceGradient(color, vec4(fogColor.xyz, fogColor.w * color.w), fogNear, fogFar, fogStrength);
}

vec4 applyVoid(vec4 color) {
	return distanceGradient(vec4(voidColor.xyz, fogColor.w * color.w), color, voidNear, voidFar, voidStrength);
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

void main(void) {
	vec4 color;
	if (textured) {
		if (useWarp) {
			vec4 warpFac = texture(warpTexture, warpUV);
			vec2 warpCoord = vec2(warpFac[warpChannelX], warpFac[warpChannelY]) * 2 - 1;
			color = texture(texture2D, fragUV + warpCoord) * fragColor;
		}
		else color = texture(texture2D, fragUV) * fragColor;
	}
	else color = fragColor;

	if (textured)
		if (color.w <= (fragColor.w * alphaClip))
			discard;

	color *= albedo;
	//color = vec4(color.xyz * albedo.xyz, 0.2);

	if (useNegative) color = vec4(vec3(1) - color.xyz, color.w);

	if (useGradient) color = applyGradient(color);

	if (useLights) color = applyLights(color);

	if (useVoid) color = applyVoid(color);

	if (useFog) color = applyFog(color);

	FragColor = color;

	//gl_FragDepth = length(fragCoord3D);
	//DepthValue = length(fragCoord3D);

	//FragColor = vec4(fragColor.x, 1, 1.1-(fragDistance/50.0), 1) * albedo;
}