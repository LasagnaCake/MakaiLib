#version 420 core

#define MAX_LIGHTS 16

uniform mat4 actor = mat4(1);
uniform mat4 world;
uniform mat4 camera;
uniform mat4 projection;

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;
layout (location = 2) in vec4 vertColor;
layout (location = 3) in vec3 vertNormal;
//layout (location = 4) in uint vertFlags;

out vec3 fragCoord3D;
out vec2 fragUV;
out vec4 fragColor;
out vec3 fragNormal;

out vec2 warpUV;

out vec3 fragLightColor;

uniform float warpRotate	= 0;
uniform vec2 warpScale		= vec2(1);
uniform vec2 warpOffset		= vec2(0);

// [ POINT LIGHTING ]
uniform bool				useLights		= false;
uniform vec3				ambientColor	= vec3(1);
uniform float				ambientStrength	= 1;
uniform uint				lightsCount		= 0;
uniform vec3[MAX_LIGHTS]	lights;
uniform vec3[MAX_LIGHTS]	lightColor;
uniform float[MAX_LIGHTS]	lightRadius;
uniform float[MAX_LIGHTS]	lightStrength;

vec3 calculateLights(vec3 position, vec3 normal) {
	return ambientColor * ambientStrength;
}

float length(in vec3 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

vec4 transformed(vec3 vec) {
	return projection * camera * world * actor * vec4(vec, 1.0);
}

void main() {
	fragColor = vertColor;
	fragUV = vertUV;
	// Warping
	vec2 warp = vertUV;
	warp.x = warp.x * cos(warpRotate) - warp.y * sin(warpRotate);
	warp.y = warp.x * sin(warpRotate) + warp.y * cos(warpRotate);
	warpUV = (warp * warpScale) + warpOffset;
	vec4 vertex	= transformed(vertPos);
	vec4 normal	= transformed(vertNormal);
	fragLightColor = calculateLights(vertex.xyz, normal.xyz);
	// Coordinates
	gl_Position	= vertex;
	fragCoord3D	= vertex.xyz;
	fragNormal	= normal.xyz;
}