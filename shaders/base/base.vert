#version 410

uniform mat4 world;
uniform mat4 camera;
uniform mat4 projection;

uniform mat4 actor;

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertColor;
layout (location = 2) in vec2 vertUV;

out vec3 fragCoord3D;

void main() {
	vec3 camPos = vec3(
		camera[0][2],
    	camera[1][2],
    	camera[2][2]
    );
    vec4 coord = projection * camera * world * actor * vec4(vertPos, 1.0);
	gl_Position = coord;
	fragCoord3D = vec3(coord.x, coord.y, coord.z);
}