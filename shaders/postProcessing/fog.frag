#version 410

uniform float   near;
uniform float   far;
uniform float   amplitude;
uniform vec4    color;

void main() {
    float fogValue  = clamp(gl_FragCoord.z, near, far);
    fogValue        = (fogValue -near) / far;
    fogValue        = fogValue / amplitude;
    gl_FragColor    = mix(gl_FragColor, color, fogValue);
}