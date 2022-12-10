#version 420 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;
layout (location = 2) in vec4 vertColor;

out vec2 fragUV;
out vec2 maskUV;
out vec4 fragColor;

uniform vec2 uvShift = vec2(0);

uniform mat4 posMatrix;
uniform mat4 uvMatrix;

// [ ALPHA MASK ]
uniform bool    relativeMask = false;
uniform vec2    maskShift = vec2(0);
uniform vec2    maskScale = vec2(1);
uniform float   maskRotate = 0;

void main()
{
    vec4 vertex = vec4(vertPos.x, vertPos.y, vertPos.z, 1.0)  * posMatrix;
    maskUV = (vertUV + maskShift) * maskScale;
    maskUV.x = maskUV.x * cos(maskRotate) - maskUV.y * sin(maskRotate);
    maskUV.y = maskUV.x * sin(maskRotate) + maskUV.y * cos(maskRotate);
    if (relativeMask) 
        maskUV += uvShift;
    fragUV = ((vec4(vertUV, 1, 0) * uvMatrix).xy + uvShift);
    fragColor = vertColor;
    gl_Position = vertex;
} 