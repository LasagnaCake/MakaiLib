#version 420 core

uniform mat4 actor = mat4(1);
uniform mat4 world;
uniform mat4 camera;
uniform mat4 projection;

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;
layout (location = 2) in vec4 vertColor;

out vec3 fragCoord3D;
out vec2 fragUV;
out vec4 fragColor;

out vec2 warpUV;

uniform float warpRotate	= 0;
uniform vec2 warpScale		= vec2(1);
uniform vec2 warpOffset		= vec2(0);

float length(in vec3 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

void main() {
    fragColor = vertColor;
    fragUV = vertUV;
    // Warping
    vec2 warp = vertUV;
    warp.x = warp.x * cos(warpRotate) - warp.y * sin(warpRotate);
    warp.y = warp.x * sin(warpRotate) + warp.y * cos(warpRotate);
    warpUV = (warp * warpScale) + warpOffset;
    vec4 coord = projection * camera * world * actor * vec4(vertPos, 1.0);
    // Coordinates
	gl_Position = coord;
	vec3 coord3D = vec3(coord.x, coord.y, coord.z);
	fragCoord3D = coord3D;
}