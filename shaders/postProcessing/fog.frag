#version 410

struct Fog {
    float   near;
    float   far;
    float   amplitude;
    vec4    color;
};

uniform Fog fog;

void main() {
    float fogValue  = clamp(gl_FragCoord.z, fog.near, fog.far);
    fogValue        = (fogValue -fog.near) / fog.far;
    fogValue        = fogValue / fog.amplitude;
    gl_FragColor       = mix(gl_FragColor, fog.color, fogValue);
}