#version 410 core

uniform mat4 world;
uniform mat4 camera;
uniform mat4 projection;

uniform mat4 actor;

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;
layout (location = 2) in vec4 vertColor;

out vec3 fragCoord3D;
out vec4 fragColor;
out vec2 fragUV;

void main() {
	vec3 camPos = vec3(
		camera[0][2],
    	camera[1][2],
    	camera[2][2]
    );
    fragColor = vec4(vertColor);
    fragUV = vec2(vertUV);
    vec4 coord = projection * camera * world * actor * vec4(vertPos, 1.0);
	gl_Position = coord;
	fragCoord3D = vec3(coord.x, coord.y, coord.z);
}