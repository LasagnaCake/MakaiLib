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
uniform float	fogFar		= 12;
uniform float	fogNear		= 8;
uniform float	fogStrength	= 1;
uniform vec4	fogColor	= vec4(1);

// [ TEXTURE WARPING ]
uniform bool		useWarp			= false;
uniform sampler2D	warpTexture;
uniform uint		warpChannelX	=	0;
uniform uint		warpChannelY	=	1;

// [ COLOR INVERSION ]
uniform bool	useNegative = false;

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
uniform vec4		ambientColor	= vec4(1);
uniform float		ambientStrength	= 1;
uniform uint 		lightsCount		= 0;
uniform vec3[255] 	lights;
uniform vec4[255] 	lightColor;
uniform float[255] 	lightStrength;

float distanceTo(in vec3 a, in vec3 b) {
	vec3 vec = a - b;
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float length(in vec3 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

vec4 normalize(in vec4 vec) {
	return vec * length(vec);
}

vec4 applyLights(in vec4 color) {
	vec4 result = vec4(1);
	for (uint i = 0; i < (lightsCount < 255 ? lightsCount : 255); i++) {
		float intensity = 1 - (distanceTo(fragCoord3D, lights[i]) / lightStrength[i]);
		intensity = clamp(intensity, 0, 1);
		vec4 albedo = mix(lightColor[i], ambientColor * ambientStrength, intensity);
		result *= albedo;
	}
	return color * result;
}

vec4 applyFog(in vec4 color) {
	// The vector's length needs to be calculated here, otherwise it breaks
	float fogValue = (length(fragCoord3D) - fogNear) / (fogFar - fogNear);
	//float fogValue = 1.0 - exp(-fragDistance * fogStrength);
	fogValue = clamp(fogValue * exp(-(1/fogStrength)), 0, 1);
	vec4 fogAlbedo = mix(color, fogColor, color.w * fogColor.w);
	fogAlbedo.w = color.w * fogColor.w;
	return mix(color, fogAlbedo, fogValue);
}

vec4 applyGradient(in vec4 color) {
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

vec4 calcFragColor(in vec4 color) {
	if (fragColor.w >= 0)
		return color * fragColor;
	else {
		vec4 result = vec4(1) - (color * fragColor);
		result.w = color.a * (-fragColor.a);
		return result; 
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

	if (useFog) color = applyFog(color);

	FragColor = color * albedo;
	//FragColor = vec4(fragColor.x, 1, 1.1-(fragDistance/50.0), 1) * albedo;
}