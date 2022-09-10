#version 410 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;
layout (location = 2) in vec4 vertColor;

out vec2 fragUV;
out vec4 fragColor;

uniform vec2 resolution;

void main()
{   
    fragUV = vertUV / resolution;
    fragColor = vertColor;
    gl_Position = vec4(vertPos.x / resolution.x, vertPos.y / resolution.y, vertPos.z, 1.0); 
} 