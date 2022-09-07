#version 410 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;
layout (location = 2) in vec4 vertColor;

out vec2 fragUV;
out vec4 fragColor;

void main()
{   
    fragUV = vertUV;
    fragColor = vertColor;
    gl_Position = vec4(vertPos.x, vertPos.y, 0.0, 1.0); 
} 