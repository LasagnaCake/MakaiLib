#version 430 core

uniform mat4 world;
uniform mat4 camera;
uniform mat4 projection;

// UV and color data not getting sent
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;
layout (location = 2) in vec4 vertColor;

out vec3 fragCoord3D;
out vec2 fragUV;
out vec4 fragColor;

out float fragDistance;

void main() {
	vec3 camPos = vec3(
		camera[0][2],
    	camera[1][2],
    	camera[2][2]
    );
    fragColor = vertColor;
    fragUV = vertUV;
    vec4 coord = projection * camera * world * vec4(vertPos, 1.0);
	gl_Position = coord;
	fragCoord3D = vec3(coord.x, coord.y, coord.z);
	fragDistance = sqrt(coord.x * coord.x + coord.y * coord.y + coord.z * coord.z);
}