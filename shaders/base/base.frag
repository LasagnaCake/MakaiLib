#version 410 core

in vec3 fragCoord3D;
in vec2 fragUV;
in vec4 fragColor;

in float fragDistance;

uniform bool textured = false;
uniform sampler2D texture2D;

uniform float alphaClip = 0.1;

uniform bool fog = false;
uniform float fogNear = 20;
uniform float fogFar = 50;
uniform float fogStrength = 1;
uniform vec4 fogColor = vec4(0);

uniform vec4 albedo = vec4(1);

void main(void) {
	vec4 color;
	if (textured)
		color = texture(texture2D, fragUV) * fragColor;
	else
		color = fragColor;
	
	if (fog) {
		float fogFactor = ((gl_FragCoord.z - fogNear) / fogFar) * fogStrength;
		fogFactor = clamp(fogFactor, 0, 1);
		color = mix(color, fogColor, fogFactor);
	}

	if (color.w < alphaClip) discard;

	gl_FragColor = mix(vec4(0), color * albedo, color.w);

	//gl_FragColor = vec4(fragColor.x, 1, 1.1-(fragDistance/50.0), 1) * albedo;
}