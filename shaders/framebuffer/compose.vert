#version 410 core

layout (location = 0) in vec4 vertPos;

void main()
{
    gl_Position = vec4(vertPos.x, vertPos.y, 0.0, 1.0); 
} 