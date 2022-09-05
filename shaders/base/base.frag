#version 410 core

in vec3 fragCoord3D;
in vec2 fragUV;
in vec4 fragColor;

in float fragDistance;

uniform bool textured;
uniform sampler2D texture2D;
uniform vec4 albedo;

void main(void) {
	vec4 color;
	if (textured)
		color = texture(texture2D, fragUV) * fragColor;
	else
		color = fragColor;
	gl_FragColor = fragColor * albedo + vec4(0, 0, 0, 1);
	//gl_FragColor = vec4(fragColor.x, 1, 1.1-(fragDistance/50.0), 1) * albedo;
}