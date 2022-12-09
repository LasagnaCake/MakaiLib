#version 420 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;
layout (location = 2) in vec4 vertColor;

out vec2 fragUV;
out vec2 maskUV;
out vec4 fragColor;

uniform vec2 resolution;
uniform vec2 uvShift = vec2(0);

// [ ALPHA MASK ]
uniform bool    relativeMask = false;
uniform vec2    maskShift = vec2(0);
uniform vec2    maskScale = vec2(1);
uniform float   maskRotate = 0;

void main()
{
    maskUV = (vertUV + maskShift) * maskScale;
    maskUV.x = maskUV.x * cos(maskRotate) - maskUV.y * sin(maskRotate);
    maskUV.y = maskUV.x * sin(maskRotate) + maskUV.y * cos(maskRotate);
    if (relativeMask) 
        maskUV += uvShift;
    fragUV = (vertUV + uvShift) / resolution;
    fragColor = vertColor;
    gl_Position = vec4(vertPos.x / resolution.x, vertPos.y / resolution.y, vertPos.z, 1.0); 
} 