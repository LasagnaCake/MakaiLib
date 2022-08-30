#version 410 core

in vec3 fragCoord3D;
in vec2 fragUV;
in vec4 fragColor;

uniform bool textured;
uniform sampler2D texture2D;

void main(void) {
	vec4 color;
	if (textured)
		color = texture(texture2D, fragUV) * fragColor;
	else
		color = fragColor;
	gl_FragColor = vec4(0, 1, 1.1-(fragCoord3D.z/50.0), 1);
}