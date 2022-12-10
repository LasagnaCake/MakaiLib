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

	struct Tuneable2D {
		Vector2
			amplitude	= Vector2(0),
			frequency	= Vector2(0),
			shift		= Vector2(0);
	};
}

namespace {
	using namespace Module;
	using Shader::Shader;
}

struct FogEffect: Effect, Limitable, ColorableRGB, Variable {};

struct TextureEffect: Effect, Imageable2D {
	float alphaClip = 0.1;
};

struct WarpEffect: Effect, Imageable2D, Transformable2D {
	unsigned int
		channelX = 0,
		channelY = 1;
};

struct NegativeEffect: Effect {};

struct GradientEffect: Effect, Channelable, Invertible {
	Vector4
		begin	= Color::BLACK,
		end		= Color::WHITE;
};

struct MaskEffect: Effect, Imageable2D, Transformable2D, Channelable, Invertible {
	bool relative = false;
};

struct WaveEffect: Effect, Tuneable2D {};

struct AmbientEffect: ColorableRGB, Variable {};

struct ObjectMaterial {
	Vector4				color = Color::WHITE;
	bool shaded			= true;
	TextureEffect	texture;
	WarpEffect		warp;
	NegativeEffect	negative;
	GradientEffect	gradient;
	bool adjustAlpha	= true;
	GLuint culling		= GL_FRONT_AND_BACK;
	GLuint fill			= GL_FILL;
};

struct BufferMaterial {
	Vector4
		color	= Color::WHITE,
		accent	= Color::NONE;
	Vector2			uvShift;
	MaskEffect		mask;
	WaveEffect		wave;
	GradientEffect	gradient;
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
	// Alpha adjust
	shader["alphaAdjust"](material.adjustAlpha);
	shader["useLights"](material.shaded);
	// Albedo
	shader["albedo"](material.color);
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
		shader["maskChannel"](material.mask.channel);
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
}

#define $mat RenderData::Material::
