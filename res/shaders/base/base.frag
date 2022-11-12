#version 420 core

in vec3 fragCoord3D;
in vec2 fragUV;
in vec4 fragColor;

in vec2 warpUV;

layout (location = 0) out vec4 FragColor;

uniform bool		textured = false;
uniform sampler2D	texture2D;
uniform float		alphaClip = 0.2;

uniform vec4 albedo = vec4(1);

uniform bool alphaAdjust = true;

uniform bool	fog			= false;
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

float length(in vec3 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
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

	if (alphaAdjust && color.w > 0)
		color = color / vec4(vec3(color.w), 1.0);

	if (fog) {
		float fogValue = (length(fragCoord3D) - fogNear) / (fogFar - fogNear);
		//float fogValue = 1.0 - exp(-fragDistance * fogStrength);
		vec4 fogAlbedo = vec4(fogColor.x, fogColor.y, fogColor.z, color.w);
		fogValue = clamp(fogValue * exp(-fogStrength), 0, 1);
		color = mix(color, fogAlbedo, fogValue);
	}
	if (useNegative) {
		float a = color.w;
		color = vec4(1) - color;
		color.w = a;
	}
	FragColor = color * albedo;
	//FragColor = vec4(fragColor.x, 1, 1.1-(fragDistance/50.0), 1) * albedo;
}