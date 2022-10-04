#version 410 core

in vec3 fragCoord3D;
in vec2 fragUV;
in vec4 fragColor;

in float fragDistance;

uniform bool textured = false;
uniform sampler2D texture2D;

uniform float alphaClip = 0.5;

uniform vec4 albedo = vec4(1);

uniform bool	fog			= false;
uniform float	fogNear		= 20;
uniform float	fogFar		= 40;
uniform float	fogStrength	= 1;
uniform vec4	fogColor	= vec4(0);

void main(void) {
	vec4 color;
	if (textured)
		color = texture(texture2D, fragUV) * fragColor;
	else
		color = fragColor;

	if (color.w <= alphaClip) discard;

	if (fog) {
		float fogValue = (fragDistance - fogNear) / fogFar;
		fogValue = clamp(fogValue, 0, 1) * fogStrength;
		color = mix(color, fogColor * vec4(vec3(1), color.w), fogValue);
	}

	gl_FragColor = mix(vec4(0), color * albedo, color.w);

	//gl_FragColor = vec4(fragColor.x, 1, 1.1-(fragDistance/50.0), 1) * albedo;
}