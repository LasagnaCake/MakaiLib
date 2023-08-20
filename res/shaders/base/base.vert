#version 420 core

#pragma optimize(on)

#define MAX_LIGHTS 16
#define MAX_INSTANCES 32

precision mediump float;

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
out vec3 fragShadeColor;

uniform vec2	uvShift	= vec2(0);

uniform float	warpRotate	= 0;
uniform vec2	warpScale	= vec2(1);
uniform vec2	warpOffset	= vec2(0);

uniform bool	shaded			= true;
uniform float	shadeIntensity	= 0.5;
uniform vec3	shadeDirection	= vec3(0, 1, 0);

// [ POINT LIGHTING ]
uniform bool				useLights		= false;
uniform vec3				ambientColor	= vec3(1);
uniform float				ambientStrength	= 1;
uniform uint				lightsCount		= 0;
uniform vec3[MAX_LIGHTS]	lights;
uniform vec3[MAX_LIGHTS]	lightColor;
uniform float[MAX_LIGHTS]	lightRadius;
uniform float[MAX_LIGHTS]	lightStrength;

// [ OBJECT INSTANCES ]
uniform vec3[MAX_INSTANCES]	instances;

vec3 calculateLights(vec3 position, vec3 normal) {
	if (!useLights) return vec3(1);
	vec3 finalColor = ambientColor * ambientStrength;
	if (lightsCount == 0) return finalColor;
	for (uint i = 0; i < (lightsCount < MAX_LIGHTS ? lightsCount : MAX_LIGHTS); i++) {
		float distance = distance(position, lights[i]);
		float attenuation = max(0.0, 1.0 - distance / lightRadius[i]);
		vec3 lightDirection = normalize(lights[i] - position);
		float diffuse = max(dot(normal, lightDirection), 0.0);
		finalColor *= lightColor[i] * lightStrength[i] * attenuation + diffuse;
	}
	return finalColor;
}

vec3 getInstancePosition() {
	return (instances[gl_InstanceID]);
}

float length(in vec3 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

vec4 transformed(vec3 vec) {
	return projection * camera * world * actor * vec4(vec + getInstancePosition(), 1.0);
}

vec3 getShadingColor(vec3 position, vec3 normal) {
	if (!shaded) return vec3(1);
	//vec3 direction = normalize(normalize(shadeDirection) - position);
	vec3 direction = normalize(shadeDirection);
	float factor = max(0, 1-dot(shadeDirection, normal));
	return vec3(1) - shadeIntensity * factor;
}

void main() {
	// Warping
	vec2 warp = vertUV;
	warp.x = warp.x * cos(warpRotate) - warp.y * sin(warpRotate);
	warp.y = warp.x * sin(warpRotate) + warp.y * cos(warpRotate);
	warpUV = (warp * warpScale) + warpOffset;
	vec4 vertex	= transformed(vertPos);
	vec3 normal	= normalize(mat3(projection * camera * world * actor) * vertNormal);
	// Point Size
	//gl_PointSize = min(0.00001, 1 - vertex.z);
	//gl_PointSize = 5.0;
	// Coordinates
	gl_Position	= vertex;
	// TODO: Proper shading
	fragShadeColor = getShadingColor(vertex.xyz, normal);
	fragColor = vertColor;
	fragLightColor = calculateLights(vertex.xyz, normal);
	fragUV = vertUV + uvShift;
	fragCoord3D	= vertex.xyz;
	fragNormal	= normal;
}