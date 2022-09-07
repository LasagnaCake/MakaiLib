#version 410 core

in vec3 fragCoord3D;
in vec2 fragUV;
in vec4 fragColor;

in float fragDistance;

uniform bool textured = false;
uniform sampler2D texture2D;

uniform float alphaMin = 0.1;

uniform vec4 albedo = vec4(1);

void main(void) {
	vec4 color;
	if (textured)
		color = texture(texture2D, fragUV) * fragColor;
	else
		color = fragColor;
	if (color.w < alphaMin) discard;
	gl_FragColor = color * albedo;
	//gl_FragColor = vec4(fragColor.x, 1, 1.1-(fragDistance/50.0), 1) * albedo;
}