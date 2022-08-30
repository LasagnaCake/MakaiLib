#version 410

in vec2 fragUV;
uniform sampler2D screen;

void main() {
//	gl_FragColor = clamp(vec4(fragUV.x + 1, fragUV.y + 1, 1, 1), 0, 1);
	gl_FragColor = texture(screen, fragUV);
}