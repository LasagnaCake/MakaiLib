#version 420 core

in vec3 fragCoord3D;
in vec2 fragUV;
in vec4 fragColor;

in vec2 warpUV;

layout (location = 0) out vec4 FragColor;

uniform uint		material = 0x00;

uniform bool		textured = false;
uniform sampler2D	texture2D;
uniform float		alphaClip = 0.2;

uniform vec4 albedo = vec4(1);

uniform bool alphaAdjust = true;

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
uniform vec4	voidColor		= vec4(vec3(0), 1);

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
uniform bool		useLights		= false;
uniform vec3		ambientColor	= vec3(1);
uniform float		ambientStrength	= 1;
uniform uint 		lightsCount		= 0;
uniform vec3[255] 	lights;
uniform vec3[255] 	lightColor;
uniform float[255] 	lightStrength;

float distanceTo(vec3 a, vec3 b) {
	vec3 vec = a - b;
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float length(vec3 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

vec4 normalize(vec4 vec) {
	return vec4(vec.xyz * length(vec), vec.w);
}

vec4 applyLights(vec4 color) {
	vec4 result = vec4(1);
	if (lightsCount == 0) return color * vec4(ambientColor, 1);
	for (uint i = 0; i < (lightsCount < 255 ? lightsCount : 255); i++) {
		float intensity = 1 - (distanceTo(fragCoord3D, lights[i]) / lightStrength[i]);
		intensity = clamp(intensity, 0, 1);
		vec3 albedo = mix(vec3(0,0,0), lightColor[i], intensity);
		result += vec4(albedo, 0);
	}
	if (length(result) > 1.0) result = normalize(result);
	return color * mix(vec4(ambientColor, 1), result, length(result));
}

vec4 distanceGradient(vec4 start, vec4 end, float near, float far, float strength) {
	// The vector's length needs to be calculated here, otherwise it breaks
	float value = (length(fragCoord3D) - near) / (far - near);
	//float fogValue = 1.0 - exp(-fragDistance * fogStrength);
	value = clamp(value * strength, 0, 1);
	vec4 albedo = mix(start, end, start.w * end.w);
	albedo.w = start.w * end.w;
	return mix(start, albedo, value);
}

vec4 applyFog(vec4 color) {
	return distanceGradient(color, fogColor, fogNear, fogFar, fogStrength);
}

vec4 applyVoid(vec4 color) {
	return distanceGradient(voidColor, color, voidNear, voidFar, voidStrength);
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

vec4 calcFragColor(vec4 color) {
	if (fragColor.w >= 0)
		return color * fragColor;
	else {
		return vec4(vec3(1) - (color.xyz * fragColor.xyz), color.a * (-fragColor.a)); 
	} 
}

void main(void) {
	vec4 color;
	if (textured) {
		if (useWarp) {
			vec4 warpFac = texture(warpTexture, warpUV);
			vec2 warpCoord = vec2(warpFac[warpChannelX], warpFac[warpChannelY]);
			color = calcFragColor(texture(texture2D, fragUV + warpCoord));
		}
		else color = calcFragColor(texture(texture2D, fragUV));
	}
	else color = calcFragColor(vec4(1));

	if (textured)
		if (color.w <= (fragColor.w * alphaClip))
			discard;

	if (useNegative) color = vec4(vec3(1) - color.xyz, color.w);

	if (useGradient) color = applyGradient(color);

	if (alphaAdjust && color.w > 0) color = color / vec4(vec3(color.w), 1.0);

	if (useLights) color = applyLights(color);

	if (useVoid) color = applyVoid(color);

	if (useFog) color = applyFog(color);

	FragColor = color * albedo;
	//FragColor = vec4(fragColor.x, 1, 1.1-(fragDistance/50.0), 1) * albedo;
}