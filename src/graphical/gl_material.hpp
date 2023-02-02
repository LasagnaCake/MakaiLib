namespace Module {
	struct Effect {
		bool enabled = false;
	};

	struct Limitable {
		float
			start	= 20.0,
			stop	= 10.0;
	};

	struct Variable {
		float	strength = 1;
	};

	struct Variable2D {
		Vector2	strength = 1;
	};

	struct Variable3D {
		Vector3	strength = 1;
	};

	struct Variable4D {
		Vector4	strength = 1;
	};

	struct ColorableRGBA {
		Vector4	color = Color::WHITE;
	};

	struct ColorableRGB {
		Vector3	color = Vector3(1);
	};

	struct Imageable2D {
		Texture2D* image = nullptr;
	};

	struct Channelable {
		int channel = -1;
	};

	struct Transformable2D {
		Transform2D trans;
	};

	struct Invertible {
		bool invert = false;
	};

	struct Tuneable {
		float
			frequency	= 0,
			amplitude	= 0,
			shift		= 0;
	};

	struct Tuneable2D {
		Vector2
			frequency	= Vector2(0),
			amplitude	= Vector2(0),
			shift		= Vector2(0);
	};

	struct Sizeable {
		float size = 0;
	};

	struct Sizeable2D {
		Vector2 size = 0;
	};
}

namespace {
	using namespace Module;
	using Shader::Shader;
}

// Generic Material Effects

struct GradientEffect: Effect, Channelable, Invertible {
	Vector4
		begin	= Color::BLACK,
		end		= Color::WHITE;
};

struct NegativeEffect: Effect {};

// Object Material Effects

struct TextureEffect: Effect, Imageable2D {
	float alphaClip = 0.1;
};

struct WarpEffect: Effect, Imageable2D, Transformable2D {
	unsigned int
		channelX = 0,
		channelY = 1;
};

// Buffer Material Effects

struct MaskEffect: Effect, Imageable2D, Transformable2D, Invertible {
	bool relative = false;
};

struct WaveEffect: Effect, Tuneable2D {
	unsigned int shape = 0;
};

struct RainbowEffect: Effect, Variable {
	float frequency	= 0.0;
	float shift		= 0.0;
	bool absoluteColor = false;
};

struct BlurEffect: Effect, Variable2D {};

struct OutlineEffect: Effect, Sizeable2D, ColorableRGBA {
	bool relativeAlpha = true;
};

// World Material Effects

struct FogEffect: Effect, Limitable, ColorableRGBA, Variable {};

struct AmbientEffect: ColorableRGB, Variable {};

// Extra Data

enum BufferDebugView: unsigned int {
	DEBUG_VIEW_NORMAL,
	DEBUG_VIEW_DEPTH
};

struct BufferDebugData: Effect {
	BufferDebugView view;
};

// Materials

struct ObjectMaterial {
	Vector4				color = Color::WHITE;
	bool shaded			= true;
	TextureEffect	texture;
	WarpEffect		warp;
	NegativeEffect	negative;
	GradientEffect	gradient;
	GLuint culling		= GL_FRONT_AND_BACK;
	GLuint fill			= GL_FILL;
};

struct BufferMaterial {
	Vector4 background = Color::CLEAR;
	Vector4
		color	= Color::WHITE,
		accent	= Color::NONE;
	Vector2			uvShift;
	MaskEffect		mask;
	NegativeEffect	negative;
	BlurEffect		blur;
	OutlineEffect	outline;
	WaveEffect		wave;
	WaveEffect		prism;
	GradientEffect	gradient;
	RainbowEffect	rainbow;
	BufferDebugData	debug;
};

struct WorldMaterial {
	FogEffect		nearFog;
	FogEffect		farFog;
	AmbientEffect	ambient;
};

void setMaterial(Shader& shader, ObjectMaterial& material) {
	// Texture
	if (material.texture.image && material.texture.enabled) {
		shader["textured"](true);
		shader["texture2D"](0);
		material.texture.image->enable(0);
	} else shader["textured"](false);
	// Texture warping
	if (material.warp.image && material.warp.enabled) {
		shader["useWarp"](true);
		shader["warpTexture"](8);
		material.warp.image->enable(8);
		shader["warpChannelX"](material.warp.channelX);
		shader["warpChannelY"](material.warp.channelY);
	} else shader["useWarp"](false);
	// Color inversion
	shader["useNegative"](material.negative.enabled);
	// Color to gradient
	shader["useGradient"](material.gradient.enabled);
	shader["gradientChannel"](material.gradient.channel);
	shader["gradientStart"](material.gradient.begin);
	shader["gradientEnd"](material.gradient.end);
	shader["gradientInvert"](material.gradient.invert);
	// Lighted
	shader["useLights"](material.shaded);
	// Albedo
	shader["albedo"](material.color);
}

void setMaterial(Shader& shader, BufferMaterial& material) {
	// Set UV data
	shader["uvShift"](material.uvShift);
	// Set color data
	shader["albedo"](material.color);
	shader["accent"](material.accent);
	// Set mask data
	if (material.mask.enabled && material.mask.image) {
		shader["useMask"](true);
		shader["mask"](0);
		material.mask.image->enable(0);
		shader["invertMask"](material.mask.invert);
		shader["relativeMask"](material.mask.relative);
		shader["maskShift"](material.mask.trans.position);
		shader["maskRotate"](material.mask.trans.rotation);
		shader["maskScale"](material.mask.trans.scale);
	} else shader["useMask"](false);
	// Set color to gradient data
	shader["useGradient"](material.gradient.enabled);
	shader["gradientChannel"](material.gradient.channel);
	shader["gradientStart"](material.gradient.begin);
	shader["gradientEnd"](material.gradient.end);
	shader["gradientInvert"](material.gradient.invert);
	// set screen wave data
	shader["useWave"](material.wave.enabled);
	shader["waveAmplitude"](material.wave.amplitude);
	shader["waveFrequency"](material.wave.frequency);
	shader["waveShift"](material.wave.shift);
	shader["waveShape"](material.wave.shape);
	// set screen prism data
	shader["usePrism"](material.prism.enabled);
	shader["prismAmplitude"](material.prism.amplitude);
	shader["prismFrequency"](material.prism.frequency);
	shader["prismShift"](material.prism.shift);
	shader["prismShape"](material.prism.shape);
	// Set color inversion
	shader["negative"](material.negative.enabled);
	// Set rainbow data
	shader["useRainbow"](material.rainbow.enabled);
	shader["rainbowFrequency"](material.rainbow.frequency);
	shader["rainbowShift"](material.rainbow.shift);
	shader["rainbowStrength"](material.rainbow.strength);
	shader["rainbowAbsolute"](material.rainbow.absoluteColor);
	// Set blur data
	shader["useBlur"](material.blur.enabled);
	shader["blurStrength"](material.blur.strength);
	// Set outline data
	shader["useOutline"](material.outline.enabled);
	shader["outlineSize"](material.outline.size);
	shader["outlineColor"](material.outline.color);
	shader["outlineMatchAlpha"](material.outline.relativeAlpha);
	// Set debug data
	shader["useDebug"](material.debug.enabled);
	shader["debugView"](material.debug.view);
}

void setMaterial(Shader& shader, WorldMaterial& material) {
	// Fog
	shader["useFog"](material.farFog.enabled);
	shader["fogNear"](material.farFog.stop);
	shader["fogFar"](material.farFog.start);
	shader["fogColor"](material.farFog.color);
	shader["fogStrength"](material.farFog.strength);
	// Void
	shader["useVoid"](material.nearFog.enabled);
	shader["voidNear"](material.nearFog.stop);
	shader["voidFar"](material.nearFog.start);
	shader["voidColor"](material.nearFog.color);
	shader["voidStrength"](material.nearFog.strength);
	// Ambient light
	shader["ambientColor"](material.ambient.color);
	shader["ambientStrength"](material.ambient.strength);
}

#define $mat RenderData::Material::
