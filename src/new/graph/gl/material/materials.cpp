#include "materials.hpp"

using namespace Makai::Graph;
using namespace Material;

void ObjectMaterial::use(Shader const& shader) const {
	// UV Data
	shader["uvShift"](uvShift);
	// Texture
	if (texture.image && texture.enabled && texture.image.exists()) {
		shader["imgTexture.enabled"](true, 0, texture.alphaClip);
		texture.image.enable(0);
	} else shader["imgTexture.enabled"](false);
	// Emission Texture
	if (emission.image && emission.enabled && emission.image.exists()) {
		shader["emission.enabled"](true, 1, emission.strength);
		emission.image.enable(1);
	} else shader["emission.enabled"](false);
	// Normal Map Texture
	if (normalMap.image && normalMap.enabled && normalMap.image.exists()) {
		shader["normalMap.enabled"](true, 2, normalMap.strength);
		normalMap.image.enable(2);
	} else shader["normalMap.enabled"](false);
	// Texture warping
	if (warp.image && warp.enabled && warp.image.exists()) {
		shader["warp.enabled"](true, 8, warp.channelX, warp.channelY);
		warp.image.enable(8);
		shader["warpTrans.position"](
			warp.trans.position,
			warp.trans.rotation,
			warp.trans.scale
		);
	} else shader["warp.enabled"](false);
	// Color inversion
	if (negative.enabled) {
		shader["negative.enabled"](true, negative.strength);
	} else shader["negative.enabled"](false);
	// Color to gradient
	if (gradient.enabled) {
		shader["gradient.enabled"](
			true,
			gradient.channel,
			gradient.begin,
			gradient.end,
			gradient.invert
		);
	} else shader["gradient.enabled"](false);
	// Lighted
	shader["shade.enabled"](shaded);
	shader["lights.enabled"](illuminated);
	// Albedo
	shader["albedo"](color);
	// HSLBC data
	shader["hue"](hue);
	shader["saturation"](saturation);
	shader["luminosity"](luminosity);
	shader["brightness"](brightness);
	shader["contrast"](contrast);
	// Instance data
	shader["instances"](instances);
	// Debug data
	shader["debugView"]((unsigned int)debug);
}

// TODO: The rest of this rat's nest
void BufferMaterial::use(Shader const& shader) const {
	// Set UV data
	shader["uvShift"](uvShift);
	// Set color data
	shader["albedo"](color);
	shader["accent"](accent);
	// Set mask data
	if (mask.enabled && mask.image && mask.image.exists()) {
		shader["useMask"](true);
		shader["mask"](9);
		mask.image.enable(9);
		shader["invertMask"](mask.invert);
		shader["relativeMask"](mask.relative);
		shader["maskShift"](mask.trans.position);
		shader["maskRotate"](mask.trans.rotation);
		shader["maskScale"](mask.trans.scale);
		shader["maskAlbedo"](mask.albedo);
		shader["maskAccent"](mask.accent);
	} else shader["useMask"](false);
	// Set texture warping data
	if (warp.image && warp.enabled && warp.image.exists()) {
		shader["useWarp"](true);
		shader["warpTexture"](8);
		warp.image.enable(8);
		shader["warpChannelX"](warp.channelX);
		shader["warpChannelY"](warp.channelY);
	} else shader["useWarp"](false);
	// Set color to gradient data
	if (gradient.enabled){
		shader["useGradient"](true);
		shader["gradientChannel"](gradient.channel);
		shader["gradientStart"](gradient.begin);
		shader["gradientEnd"](gradient.end);
		shader["gradientInvert"](gradient.invert);
	} else shader["useGradient"](false);
	// set screen wave data
	if (wave.enabled) {
		shader["useWave"](true);
		shader["waveAmplitude"](wave.amplitude);
		shader["waveFrequency"](wave.frequency);
		shader["waveShift"](wave.shift);
		shader["waveShape"](wave.shape);
	} else shader["useWave"](false);
	// set screen prism data
	if (prism.enabled) {
		shader["usePrism"](true);
		shader["prismAmplitude"](prism.amplitude);
		shader["prismFrequency"](prism.frequency);
		shader["prismShift"](prism.shift);
		shader["prismShape"](prism.shape);
	} else shader["usePrism"](false);
	// Set color inversion
	if (negative.enabled) {
		shader["useNegative"](true);
		shader["negativeStrength"](negative.strength);
	} else shader["useNegative"](false);
	// Set rainbow data
	if (rainbow.enabled) {
		shader["useRainbow"](true);
		shader["rainbowFrequency"](rainbow.frequency);
		shader["rainbowShift"](rainbow.shift);
		shader["rainbowStrength"](rainbow.strength);
		shader["rainbowAbsolute"](rainbow.absoluteColor);
		shader["rainbowPolar"](rainbow.polar);
		shader["rainbowPolarShift"](rainbow.polarShift);
	} else shader["useRainbow"](false);
	// Set blur data
	if (blur.enabled) {
		shader["useBlur"](true);
		shader["blurStrength"](blur.strength);
	} else shader["useBlur"](false);
	// Set polar warp data
	if (polarWarp.enabled) {
		shader["usePolarWarp"](true);
		shader["polarWarpStrength"](polarWarp.strength);
		shader["polarWarpSize"](polarWarp.size);
		shader["polarWarpPosition"](polarWarp.position);
		shader["polarWarpColor"](polarWarp.color);
		shader["polarWarpTintStrength"](polarWarp.tintStrength);
		shader["polarWarpFishEye"](polarWarp.fishEye);
	} else shader["usePolarWarp"](false);
	// Set outline data
	if (outline.enabled) {
		shader["useOutline"](true);
		shader["outlineSize"](outline.size);
		shader["outlineColor"](outline.color);
		shader["outlineMatchAlpha"](outline.relativeAlpha);
	} else shader["useOutline"](false);
	// Set noise data
	if (noise.enabled) {
		shader["useNoise"](true);
		shader["noiseOffset"](noise.trans.position);
		//shader["noiseRotation"](noise.trans.rotation);
		shader["noiseStrength"](noise.trans.scale);
		shader["noiseScale"](noise.strength);
		shader["noiseSeed"](noise.seed);
		shader["noiseType"]((unsigned int)noise.type);
		shader["noiseBlendSrcColorFunc"]((unsigned int)noise.blend.color.source);
		shader["noiseBlendDstColorFunc"]((unsigned int)noise.blend.color.destination);
		shader["noiseBlendColorEq"]((unsigned int)noise.blend.color.equation);
		shader["noiseBlendSrcAlphaFunc"]((unsigned int)noise.blend.alpha.source);
		shader["noiseBlendDstAlphaFunc"]((unsigned int)noise.blend.alpha.destination);
		shader["noiseBlendAlphaEq"]((unsigned int)noise.blend.alpha.equation);
	} else shader["useNoise"](false);
	// Set debug data
	shader["debugView"]((unsigned int)debug);
	// Set HSLBC data
	shader["hue"](hue);
	shader["saturation"](saturation);
	shader["luminosity"](luminosity);
	shader["brightness"](brightness);
	shader["contrast"](contrast);
}

void WorldMaterial::use(Shader const& shader) const {
	// Fog
	if (farFog.enabled) {
		shader["farFog.enabled"](
			true,
			farFog.start,
			farFog.stop,
			farFog.strength
		);
		shader["farFog.color"](farFog.color);
	} else shader["farFog.enabled"](false);
	// Void
	if (nearFog.enabled) {
		shader["nearFog.enabled"](
			true,
			nearFog.start,
			nearFog.stop,
			nearFog.strength
		);
		shader["nearFog.color"](nearFog.color);
	} else shader["nearFog.enabled"](false);
	// Ambient light
	shader["ambient.color"](ambient.color, ambient.strength);
}
