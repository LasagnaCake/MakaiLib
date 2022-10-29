#version 410

uniform sampler2D depth;
uniform sampler2D screen;

uniform float   near = 20;
uniform float   far = 40;
uniform float   amplitude = 1;
uniform vec4    color = vec4(0);

in vec2 fragUV;

void main() {
    float fogValue  = clamp(gl_FragCoord.z, near, far);
    fogValue        = (fogValue -near) / far;
    fogValue        = fogValue / amplitude;
    gl_FragColor    = vec4(gl_FragCoord.z,0,gl_FragCoord.z,1);
}