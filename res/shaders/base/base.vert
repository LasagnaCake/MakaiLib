#version 420 core

#define MAX_LIGHTS 16

uniform mat4 actor = mat4(1);
uniform mat4 world;
uniform mat4 camera;
uniform mat4 projection;

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;
layout (location = 2) in vec4 vertColor;
//layout (location = 3) in uint vertFlags;

out vec3 fragCoord3D;
out vec2 fragUV;
out vec4 fragColor;

out vec2 warpUV;

out vec3 fragLightColor;

uniform float warpRotate	= 0;
uniform vec2 warpScale		= vec2(1);
uniform vec2 warpOffset		= vec2(0);

// [ POINT LIGHTING ]
uniform bool				useLights		= false;
uniform vec3				ambientColor	= vec3(1);
uniform float				ambientStrength	= 1;
uniform uint 				lightsCount		= 0;
uniform vec3[MAX_LIGHTS] 	lights;
uniform vec3[MAX_LIGHTS] 	lightColor;
uniform float[MAX_LIGHTS] 	lightRadius;
uniform float[MAX_LIGHTS] 	lightStrength;

/*	TODO: Test this. This was made with ChatGPT.
*	This will be the first an last time I use it.
*/
vec3 calculateLights(vec3 vertexPos, vec3 ambientColor, float ambientStrength) {
	// Calculate the ambient color contribution
	vec3 color = ambientColor * ambientStrength;
	// Loop over all point lights and calculate their contribution
	if (lightsCount > 0) for (uint i = 0; i < min(lightsCount, uint(lights.length()-1)); i++) {
		vec3 lightPos = lights[i];
		vec3 lightColor = lightColor[i];
		float lightStrength = lightStrength[i];
		float lightRadius = lightRadius[i];
		// Calculate the distance between the vertex and the point light
		float dist = distance(vertexPos, lightPos);
		// Calculate the attenuation factor based on the light's radius and distance from the vertex
		float attenuation = 1.0 / (1.0 + dist * dist) / (lightRadius * lightRadius);
		// Calculate the color contribution from the current point light
		vec3 lightContribution = lightColor * lightStrength * attenuation;
		// Add the contribution to the total color
		color += lightContribution;
	}
	// Return the final calculated color
	return color;
}

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
	fragLightColor = calculateLights(coord.xyz, ambientColor, ambientStrength);
	// Coordinates
	gl_Position = coord;
	vec3 coord3D = vec3(coord.x, coord.y, coord.z);
	fragCoord3D = coord3D;
}