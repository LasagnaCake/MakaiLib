namespace Module {
	struct MODULAR Effect {
		bool enabled = false;
	};

	struct MODULAR Limitable {
		float
			start	= 0.0,
			stop	= 10.0;
	};

	struct MODULAR Variable {
		float	strength = 1;
	};

	struct MODULAR Variable2D {
		Vector2	strength = 1;
	};

	struct MODULAR Variable3D {
		Vector3	strength = 1;
	};

	struct MODULAR Variable4D {
		Vector4	strength = 1;
	};

	struct MODULAR ColorableRGBA {
		Vector4	color = Color::WHITE;
	};

	struct MODULAR ColorableRGB {
		Vector3	color = 1;
	};

	struct MODULAR Imageable2D {
		Texture2D* image = nullptr;
	};

	struct MODULAR Channelable {
		int channel = -1;
	};

	struct MODULAR Transformable2D {
		Transform2D trans;
	};

	struct MODULAR Positionable2D {
		Vector2 position;
	};

	struct MODULAR Invertible {
		bool invert = false;
	};

	struct MODULAR Tuneable {
		float
			frequency	= 0,
			amplitude	= 0,
			shift		= 0;
	};

	struct MODULAR Tuneable2D {
		Vector2
			frequency	= Vector2(0),
			amplitude	= Vector2(0),
			shift		= Vector2(0);
	};

	struct MODULAR Sizeable {
		float size = 0;
	};

	struct MODULAR Sizeable2D {
		Vector2 size = 0;
	};

	struct MODULAR Countable {
		size_t count = 1;
	};
}

namespace {
	using namespace Module;
	using Shader::Shader;
}

// Generic Material Effects

enum class BlendFunction: unsigned int {
	BF_ZERO = 0,
	BF_ONE,
	BF_SRC,
	BF_ONE_MINUS_SRC,
	BF_DST,
	BF_ONE_MINUS_DST,
};

enum class BlendEquation {
	BE_ADD,
	BE_SUBTRACT,
	BE_MULTIPLY,
	BE_DIVIDE,
	BE_REVERSE_SUBTRACT,
	BE_REVERSE_DIVIDE,
};

struct BlendMode {
	BlendFunction source		= BlendFunction::BF_SRC;
	BlendFunction destination	= BlendFunction::BF_DST;
	BlendEquation equation		= BlendEquation::BE_MULTIPLY;
};

struct BlendSetting {
	BlendMode	color	= {BlendFunction::BF_ONE, BlendFunction::BF_DST, BlendEquation::BE_MULTIPLY};
	BlendMode	alpha	= {BlendFunction::BF_SRC, BlendFunction::BF_ONE, BlendEquation::BE_MULTIPLY};
};

struct GradientEffect: Effect, Channelable, Invertible {
	Vector4
		begin	= Color::BLACK,
		end		= Color::WHITE;
};

struct NegativeEffect: Effect, Variable {};

struct ImageEffect: Effect, Imageable2D {};

struct WarpEffect: ImageEffect, Transformable2D {
	unsigned int
		channelX = 0,
		channelY = 1;
};

// Object Material Effects

struct TextureEffect: ImageEffect {
	float alphaClip = 0.1;
};

struct EmissionEffect: TextureEffect, Variable {
};

// Buffer Material Effects

struct MaskEffect: Effect, Imageable2D, Transformable2D, Invertible {
	Vector4	albedo		= 1;
	Vector4	accent		= 0;
	bool	relative	= false;
};

enum WaveShape: unsigned int {
	WS_SQUARE = 0,
	WS_SINE,
	WS_BIN_SINE,
	WS_ABS_SINE,
	WS_ABS_BIN_SINE,
	WS_TRIANGLE,
	WS_BIN_TRIANGLE,
	WS_ABS_TRIANGLE,
	WS_ABS_BIN_TRIANGLE,
	WS_HALF_SINE,
	WS_BIN_HALF_SINE,
	WS_ABS_HALF_SINE,
	WS_ABS_BIN_HALF_SINE,
	WS_HALF_TRIANGLE,
	WS_BIN_HALF_TRIANGLE,
	WS_ABS_HALF_TRIANGLE,
	WS_ABS_BIN_HALF_TRIANGLE,
	WS_SIMPLE_NOISE,
	WS_BIN_SIMPLE_NOISE,
};

struct WaveEffect: Effect, Tuneable2D {
	WaveShape	shape	= WS_SQUARE;
};

struct RainbowEffect: Effect, Variable {
	Vector2 frequency		= 0.0;
	Vector2 shift			= 0.0;
	bool	absoluteColor	= false;
	bool	polar			= false;
	float	polarShift		= 0.0;
};

struct BlurEffect: Effect, Variable2D {};

struct OutlineEffect: Effect, Sizeable2D, ColorableRGBA {
	bool relativeAlpha = true;
};

struct PolarWarpEffect: Effect, Sizeable, Positionable2D, Variable2D, ColorableRGBA {
	float	tintStrength	= 1;
	bool	fishEye			= true;
};

enum class NoiseType: unsigned int {
	NT_NOISE_SIMPLE = 0,
	NT_NOISE_GOLD,
	NT_NOISE_SUPER
};

/// SRC = Pixel Color, DST = Noise
struct NoiseBlendMode: BlendSetting {};

struct NoiseEffect: Effect, Variable, Transformable2D {
	float			seed	= 1;
	NoiseType		type	= NoiseType::NT_NOISE_SUPER;
	NoiseBlendMode	blend;
};

// World Material Effects

struct FogEffect: Effect, Limitable, ColorableRGBA, Variable {};

struct AmbientEffect: ColorableRGB, Variable {};

// Extra Data

enum class BufferDebugView: unsigned int {
	BDV_NONE = 0,
	BDV_DEPTH,
};

enum class ObjectDebugView: unsigned int {
	ODV_NONE = 0,
	ODV_NORMAL,
};

// Materials

struct ObjectMaterial {
	Vector4				color = Color::WHITE;
	bool shaded			= false;
	bool illuminated	= false;
	float			hue			= 0;
	float			saturation	= 1;
	float			luminosity	= 1;
	float			brightness	= 0;
	float			contrast	= 1;
	Vector2			uvShift;
	TextureEffect	texture;
	EmissionEffect	emission;
	WarpEffect		warp;
	NegativeEffect	negative;
	GradientEffect	gradient;
	List<Vector3>	instances = {Vec3(0, 0, 0)};
	GLuint culling		= GL_FRONT_AND_BACK;
	GLuint fill			= GL_FILL;
	ObjectDebugView	debug	= ObjectDebugView::ODV_NONE;
};

struct BufferMaterial {
	Vector4 background = Color::NONE;
	Vector4
		color	= Color::WHITE,
		accent	= Color::NONE;
	float			hue			= 0;
	float			saturation	= 1;
	float			luminosity	= 1;
	float			brightness	= 0;
	float			contrast	= 1;
	Vector2			uvShift;
	MaskEffect		mask;
	WarpEffect		warp;
	NegativeEffect	negative;
	BlurEffect		blur;
	OutlineEffect	outline;
	WaveEffect		wave;
	WaveEffect		prism;
	PolarWarpEffect	polarWarp;
	GradientEffect	gradient;
	RainbowEffect	rainbow;
	NoiseEffect		noise;
	BufferDebugView	debug	= BufferDebugView::BDV_NONE;
};

struct WorldMaterial {
	FogEffect		nearFog;
	FogEffect		farFog;
	AmbientEffect	ambient;
};

void setMaterial(Shader& shader, ObjectMaterial& material) {
	// UV Data
	shader["uvShift"](material.uvShift);
	// Texture
	if (material.texture.image && material.texture.enabled && material.texture.image->exists()) {
		shader["textured"](true);
		shader["alphaClip"](material.texture.alphaClip);
		shader["texture2D"](0);
		material.texture.image->enable(0);
	} else shader["textured"](false);
	// Emission Texture
	if (material.emission.image && material.emission.enabled && material.emission.image->exists()) {
		shader["useEmission"](true);
		shader["emissionAlphaClip"](material.emission.alphaClip);
		shader["emissionTexture"](1);
		shader["emissionStrength"](material.emission.strength);
		material.emission.image->enable(1);
	} else shader["useEmission"](false);
	// Texture warping
	if (material.warp.image && material.warp.enabled && material.warp.image->exists()) {
		shader["useWarp"](true);
		shader["warpTexture"](8);
		material.warp.image->enable(8);
		shader["warpChannelX"](material.warp.channelX);
		shader["warpChannelY"](material.warp.channelY);
		shader["warpOffset"](material.warp.trans.position);
		shader["warpScale"](material.warp.trans.scale);
		shader["warpRotate"](material.warp.trans.rotation);
	} else shader["useWarp"](false);
	// Color inversion
	if (material.negative.enabled) {
		shader["useNegative"](true);
		shader["negativeStrength"](material.negative.strength);
	} else shader["useNegative"](false);
	// Color to gradient
	if (material.gradient.enabled) {
		shader["useGradient"](true);
		shader["gradientChannel"](material.gradient.channel);
		shader["gradientStart"](material.gradient.begin);
		shader["gradientEnd"](material.gradient.end);
		shader["gradientInvert"](material.gradient.invert);
	} else shader["useGradient"](false);
	// Lighted
	shader["shaded"](material.shaded);
	shader["useLights"](material.illuminated);
	// Albedo
	shader["albedo"](material.color);
	// HSLBC data
	shader["hue"](material.hue);
	shader["saturation"](material.saturation);
	shader["luminosity"](material.luminosity);
	shader["brightness"](material.brightness);
	shader["contrast"](material.contrast);
	// Instance data
	shader["instances"](material.instances);
	// Debug data
	shader["debugView"]((unsigned int)material.debug);
}

void setMaterial(Shader& shader, BufferMaterial& material) {
	// Set UV data
	shader["uvShift"](material.uvShift);
	// Set color data
	shader["albedo"](material.color);
	shader["accent"](material.accent);
	// Set mask data
	if (material.mask.enabled && material.mask.image && material.mask.image->exists()) {
		shader["useMask"](true);
		shader["mask"](9);
		material.mask.image->enable(9);
		shader["invertMask"](material.mask.invert);
		shader["relativeMask"](material.mask.relative);
		shader["maskShift"](material.mask.trans.position);
		shader["maskRotate"](material.mask.trans.rotation);
		shader["maskScale"](material.mask.trans.scale);
		shader["maskAlbedo"](material.mask.albedo);
		shader["maskAccent"](material.mask.accent);
	} else shader["useMask"](false);
	// Set texture warping data
	if (material.warp.image && material.warp.enabled && material.warp.image->exists()) {
		shader["useWarp"](true);
		shader["warpTexture"](8);
		material.warp.image->enable(8);
		shader["warpChannelX"](material.warp.channelX);
		shader["warpChannelY"](material.warp.channelY);
	} else shader["useWarp"](false);
	// Set color to gradient data
	if (material.gradient.enabled){
		shader["useGradient"](true);
		shader["gradientChannel"](material.gradient.channel);
		shader["gradientStart"](material.gradient.begin);
		shader["gradientEnd"](material.gradient.end);
		shader["gradientInvert"](material.gradient.invert);
	} else shader["useGradient"](false);
	// set screen wave data
	if (material.wave.enabled) {
		shader["useWave"](true);
		shader["waveAmplitude"](material.wave.amplitude);
		shader["waveFrequency"](material.wave.frequency);
		shader["waveShift"](material.wave.shift);
		shader["waveShape"](material.wave.shape);
	} else shader["useWave"](false);
	// set screen prism data
	if (material.prism.enabled) {
		shader["usePrism"](true);
		shader["prismAmplitude"](material.prism.amplitude);
		shader["prismFrequency"](material.prism.frequency);
		shader["prismShift"](material.prism.shift);
		shader["prismShape"](material.prism.shape);
	} else shader["usePrism"](false);
	// Set color inversion
	if (material.negative.enabled) {
		shader["useNegative"](true);
		shader["negativeStrength"](material.negative.strength);
	} else shader["useNegative"](false);
	// Set rainbow data
	if (material.rainbow.enabled) {
		shader["useRainbow"](true);
		shader["rainbowFrequency"](material.rainbow.frequency);
		shader["rainbowShift"](material.rainbow.shift);
		shader["rainbowStrength"](material.rainbow.strength);
		shader["rainbowAbsolute"](material.rainbow.absoluteColor);
		shader["rainbowPolar"](material.rainbow.polar);
		shader["rainbowPolarShift"](material.rainbow.polarShift);
	} else shader["useRainbow"](false);
	// Set blur data
	if (material.blur.enabled) {
		shader["useBlur"](true);
		shader["blurStrength"](material.blur.strength);
	} else shader["useBlur"](false);
	// Set polar warp data
	if (material.polarWarp.enabled) {
		shader["usePolarWarp"](true);
		shader["polarWarpStrength"](material.polarWarp.strength);
		shader["polarWarpSize"](material.polarWarp.size);
		shader["polarWarpPosition"](material.polarWarp.position);
		shader["polarWarpColor"](material.polarWarp.color);
		shader["polarWarpTintStrength"](material.polarWarp.tintStrength);
		shader["polarWarpFishEye"](material.polarWarp.fishEye);
	} else shader["usePolarWarp"](false);
	// Set outline data
	if (material.outline.enabled) {
		shader["useOutline"](true);
		shader["outlineSize"](material.outline.size);
		shader["outlineColor"](material.outline.color);
		shader["outlineMatchAlpha"](material.outline.relativeAlpha);
	} else shader["useOutline"](false);
	// Set noise data
	if (material.noise.enabled) {
		shader["useNoise"](true);
		shader["noiseOffset"](material.noise.trans.position);
		//shader["noiseRotation"](material.noise.trans.rotation);
		shader["noiseStrength"](material.noise.trans.scale);
		shader["noiseScale"](material.noise.strength);
		shader["noiseSeed"](material.noise.seed);
		shader["noiseType"]((unsigned int)material.noise.type);
		shader["noiseBlendSrcColorFunc"]((unsigned int)material.noise.blend.color.source);
		shader["noiseBlendDstColorFunc"]((unsigned int)material.noise.blend.color.destination);
		shader["noiseBlendColorEq"]((unsigned int)material.noise.blend.color.equation);
		shader["noiseBlendSrcAlphaFunc"]((unsigned int)material.noise.blend.alpha.source);
		shader["noiseBlendDstAlphaFunc"]((unsigned int)material.noise.blend.alpha.destination);
		shader["noiseBlendAlphaEq"]((unsigned int)material.noise.blend.alpha.equation);
	} else shader["useNoise"](false);
	// Set debug data
	shader["debugView"]((unsigned int)material.debug);
	// Set HSLBC data
	shader["hue"](material.hue);
	shader["saturation"](material.saturation);
	shader["luminosity"](material.luminosity);
	shader["brightness"](material.brightness);
	shader["contrast"](material.contrast);
}

void setMaterial(Shader& shader, WorldMaterial& material) {
	// Fog
	if (material.farFog.enabled) {
		shader["useFog"](true);
		shader["fogNear"](material.farFog.start);
		shader["fogFar"](material.farFog.stop);
		shader["fogColor"](material.farFog.color);
		shader["fogStrength"](material.farFog.strength);
	} else shader["useFog"](false);
	// Void
	if (material.nearFog.enabled) {
		shader["useVoid"](true);
		shader["voidNear"](material.nearFog.start);
		shader["voidFar"](material.nearFog.stop);
		shader["voidColor"](material.nearFog.color);
		shader["voidStrength"](material.nearFog.strength);
	} else shader["useVoid"](false);
	// Ambient light
	shader["ambientColor"](material.ambient.color);
	shader["ambientStrength"](material.ambient.strength);
}

JSONData saveImageEffect(ImageEffect& effect, String const& folder, String const& path) {
	JSONData def;
	def["enabled"] = effect.enabled;
	if (effect.image && effect.image->exists()) {
		effect.image->saveToFile(FileSystem::concatenatePath(folder, path));
		def["image"] = {
			{"path", path},
			{"minFilter", effect.image->getTextureMinFilter()},
			{"magFilter", effect.image->getTextureMagFilter()}
		};
	} else def["enabled"] = false;
	return def;
}

ImageEffect loadImageEffect(
	JSONData& effect,
	String const& sourcepath,
	Texture2D* texture
) {
	try {
		ImageEffect fx;
		fx.enabled = effect["enabled"].get<bool>();
		auto& img = effect["image"];
		if (!texture)
			texture = new Texture2D();
		fx.image = texture;
		if (img["data"].is_object() && img["data"]["path"].is_string() && !img["data"]["path"].get<string>().empty()) {
			texture->create(FileSystem::concatenatePath(sourcepath, img["path"].get<string>()));
			texture->setTextureFilterMode(
				img["minFilter"].get<unsigned int>(),
				img["magFilter"].get<unsigned int>()
			);
		} else if (img["data"].is_string() && !img["data"].get<string>().empty()) {
			vector<ubyte> data = decodeData(img["data"].get<string>(), img["encoding"]);
			int w, h, nc;
			uchar* imgdat = stbi_load_from_memory(
				data.data(),
				data.size(),
				&w,
				&h,
				&nc,
				4
			);
			if (imgdat) {
				texture->create(
					w,
					h,
					GL_UNSIGNED_BYTE,
					GL_RGBA,
					img["magFilter"].get<unsigned int>(),
					img["minFilter"].get<unsigned int>(),
					imgdat
				);
				stbi_image_free(imgdat);
			} else throw Error::FailedAction(
					"Failed at getting image effect!",
					__FILE__,
					toString(__LINE__),
					"loadImageEffect",
					"Could not decode embedded image data!",
					"Please check to see if values are correct!"
				);
		} else fx.enabled = false;
		return fx;
	} catch (JSON::exception e) {
		throw Error::FailedAction(
			"Failed at getting image effect!",
			__FILE__,
			toString(__LINE__),
			"loadImageEffect",
			e.what(),
			"Please check to see if values are correct!"
		);
	}
}

ObjectMaterial fromObjectMaterialDefinition(
	JSONData def,
	String const& definitionFolder,
	Texture2D* texture,
	Texture2D* emission,
	Texture2D* warp
) {
	ObjectMaterial mat;
	try {
		auto& dmat = def;
		// Set color
		if(dmat["color"].is_array()) {
			mat.color.x = dmat["color"][0].get<float>();
			mat.color.y = dmat["color"][1].get<float>();
			mat.color.z = dmat["color"][2].get<float>();
			mat.color.w = dmat["color"][3].get<float>();
		}
		else if (dmat["color"].is_string())
			mat.color = Color::fromHexCodeString(dmat["color"].get<String>());
		// Set color & shading params
		#define _SET_BOOL_PARAM(PARAM) if(dmat[#PARAM].is_boolean()) mat.PARAM = dmat[#PARAM].get<bool>()
		_SET_BOOL_PARAM(shaded);
		_SET_BOOL_PARAM(illuminated);
		#undef _SET_BOOL_PARAM
		#define _SET_FLOAT_PARAM(PARAM) if(dmat[#PARAM].is_number()) mat.PARAM = dmat[#PARAM].get<float>()
		_SET_FLOAT_PARAM(hue);
		_SET_FLOAT_PARAM(saturation);
		_SET_FLOAT_PARAM(luminosity);
		_SET_FLOAT_PARAM(brightness);
		_SET_FLOAT_PARAM(contrast);
		#undef _SET_FLOAT_PARAM
		// Set UV shift
		if(dmat["uvShift"].is_array()) {
			mat.uvShift.x = dmat["uvShift"][0].get<float>();
			mat.uvShift.y = dmat["uvShift"][1].get<float>();
		}
		// Set texture
		if (dmat["texture"].is_object()) {
			auto fx = loadImageEffect(dmat["texture"], definitionFolder, texture ? texture : mat.texture.image);
			mat.texture.enabled	= fx.enabled;
			mat.texture.image		= fx.image;
			if (dmat["texture"]["alphaClip"].is_number())
				mat.texture.alphaClip	= dmat["texture"]["alphaClip"].get<float>();
		}
		// Set emission texture
		if (dmat["emission"].is_object()) {
			auto fx = loadImageEffect(dmat["emission"], definitionFolder, emission ? emission : mat.emission.image);
			mat.emission.enabled	= fx.enabled;
			mat.emission.image		= fx.image;
			if (dmat["emission"]["alphaClip"].is_number())
				mat.emission.alphaClip	= dmat["emission"]["alphaClip"].get<float>();
			if (dmat["emission"]["strength"].is_number())
				mat.emission.alphaClip	= dmat["emission"]["strength"].get<float>();
		}
		// Set warp texture
		if (dmat["warp"].is_object()) {
			auto fx = loadImageEffect(dmat["warp"], definitionFolder, warp ? warp : mat.warp.image);
			mat.warp.enabled	= fx.enabled;
			mat.warp.image		= fx.image;
			{
				auto& mwtrans = dmat["warp"]["trans"];
				mat.warp.trans.position = Vector2(
					mwtrans["position"][0].get<float>(),
					mwtrans["position"][1].get<float>()
				);
				mat.warp.trans.rotation = mwtrans["rotation"].get<float>();
				mat.warp.trans.scale = Vector2(
					mwtrans["scale"][0].get<float>(),
					mwtrans["scale"][1].get<float>()
				);
			}
			mat.warp.channelX = dmat["warp"]["channelX"];
			mat.warp.channelY = dmat["warp"]["channelY"];
		}
		// Set negative
		if (dmat["negative"].is_object()) {
			mat.negative.enabled	= dmat["negative"]["enabled"].get<bool>();
			mat.negative.strength	= dmat["negative"]["strength"].get<float>();
		}
		// Set gradient
		if (dmat["gradient"].is_object()) {
			mat.gradient.enabled	= dmat["gradient"]["enabled"].get<bool>();
			mat.gradient.channel	= dmat["gradient"]["channel"].get<unsigned int>();
			auto& dgbegin	= dmat["gradient"]["begin"];
			auto& dgend		= dmat["gradient"]["end"];
			if (dgbegin.is_array())
				mat.gradient.begin = Vector4(
					dgbegin[0].get<float>(),
					dgbegin[1].get<float>(),
					dgbegin[2].get<float>(),
					dgbegin[3].get<float>()
				);
			else if (dgbegin.is_string())
				mat.gradient.begin = Color::fromHexCodeString(dgbegin.get<String>());
			if (dgend.is_array())
				mat.gradient.end = Vector4(
					dgend[0].get<float>(),
					dgend[1].get<float>(),
					dgend[2].get<float>(),
					dgend[3].get<float>()
				);
			else if (dgend.is_string())
				mat.gradient.end = Color::fromHexCodeString(dgend.get<String>());
			mat.gradient.invert	= dmat["gradient"]["invert"].get<bool>();
		}
		// Set instances
		if (dmat["instances"].is_array()) {
			mat.instances.clear();
			for(auto& inst: dmat["instances"])
				mat.instances.push_back(
					Vector3(
						inst[0].get<float>(),
						inst[1].get<float>(),
						inst[2].get<float>()
					)
				);
		}
		// Set culling, fill & view
		if (dmat["culling"].is_number()) {
			switch (dmat["culling"].get<unsigned int>()) {
				default:
				case 0: mat.culling = GL_FRONT_AND_BACK;	break;
				case 1: mat.culling = GL_FRONT;				break;
				case 2: mat.culling = GL_BACK;				break;
			}
		}
		if (dmat["fill"].is_number()) {
			switch (dmat["fill"].get<unsigned int>()) {
				default:
				case 0: mat.fill = GL_FILL;		break;
				case 1: mat.fill = GL_LINE;		break;
				case 2: mat.fill = GL_POINT;	break;
			}
		}
		if (dmat["debug"].is_number())
			mat.debug = (ObjectDebugView)dmat["debug"].get<unsigned int>();
	} catch (JSON::exception e) {
		throw Error::FailedAction(
			"Failed at getting material values!",
			__FILE__,
			toString(__LINE__),
			"extendFromDefinition",
			e.what(),
			"Please check to see if values are correct!"
		);
	}
	return mat;
}

JSONData getMaterialDefinition(
	ObjectMaterial& mat,
	String const& definitionFolder,
	String const& texturesFolder,
	bool integratedTextures = false
) {
	JSONData def;
	// Copy instances
	List<JSONData> instanceData;
	for (Vector3& inst: mat.instances) {
		instanceData.push_back({inst.x, inst.y, inst.z});
	}
	// Define object
	def = {
		{"color", Color::toHexCodeString(mat.color, false, true)},
		{"shaded", mat.shaded},
		{"illuminated", mat.illuminated},
		{"hue", mat.hue},
		{"saturation", mat.saturation},
		{"luminosity", mat.luminosity},
		{"brightness", mat.brightness},
		{"contrast", mat.contrast},
		{"uvShift", {mat.uvShift.x, mat.uvShift.y}},
		{"negative", {
			{"enabled", mat.negative.enabled},
			{"strength", mat.negative.strength}
		}},
		{"gradient", {
			{"enabled", mat.gradient.enabled},
			{"channel", mat.gradient.channel},
			{"begin", {mat.gradient.begin.x, mat.gradient.begin.y, mat.gradient.begin.z, mat.gradient.begin.w}},
			{"end", {mat.gradient.end.x, mat.gradient.end.y, mat.gradient.end.z, mat.gradient.end.w}},
			{"invert", mat.gradient.invert}
		}},
		{"instances", instanceData},
		{"debugView", (unsigned int)mat.debug}
	};
	switch (mat.fill) {
		case GL_FILL:	def["material"]["fill"] = 0; break;
		case GL_LINE:	def["material"]["fill"] = 1; break;
		case GL_POINT:	def["material"]["fill"] = 2; break;
	}
	switch (mat.culling) {
		case GL_FRONT_AND_BACK:	def["material"]["culling"] = 0; break;
		case GL_FRONT:			def["material"]["culling"] = 1; break;
		case GL_BACK:			def["material"]["culling"] = 2; break;
	}
	// Save image texture
	if (!integratedTextures) {
		def["warp"]		= saveImageEffect(mat.warp, definitionFolder, texturesFolder + "/warp.tga");
		def["texture"]	= saveImageEffect(mat.texture, definitionFolder, texturesFolder + "/texture.tga");
		def["emission"]	= saveImageEffect(mat.emission, definitionFolder, texturesFolder + "/emission.tga");
	} else {
		// TODO: integrated textures
	}
	// Set stuff
	def["texture"]["alphaClip"] = mat.texture.alphaClip;
	def["emission"]["alphaClip"] = mat.emission.alphaClip;
	def["emission"]["strength"] = mat.emission.strength;
	def["warp"]["channelX"] = mat.warp.channelX;
	def["warp"]["channelY"] = mat.warp.channelY;
	def["warp"]["trans"] = {
		{"position",	{mat.warp.trans.position.x,	mat.warp.trans.position.y	}	},
		{"rotation",	mat.warp.trans.rotation										},
		{"scale",		{mat.warp.trans.scale.x,	mat.warp.trans.scale.y		}	}
	};
	// Return definition
	return def;
}
