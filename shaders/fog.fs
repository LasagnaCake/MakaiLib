struct Fog {
    float   near;
    float   far;
    float   amplitude;
    vec4    color;
};

uniform Fog fog;

inout fragColor;

void main() {
    float fogValue  = clamp(gl_FragCoord.z, fog.near, fog.far);
    fogValue        = fog.far / (fogValue -fog.near);
    fogValue        = lerp(0, 1, fogValue / fog.amplitude);
    fragColor       = mix(fragColor, fog.color, fogValue)
}