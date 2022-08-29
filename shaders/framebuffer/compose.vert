#version 410 core

layout (location = 0) in vec4 vertPos;
layout (location = 1) in vec2 vertUV;

out vec2 fragUV;

void main()
{   
    fragUV = vertUV;
    gl_Position = vec4(vertPos.x, vertPos.y, 0.0, 1.0); 
} 