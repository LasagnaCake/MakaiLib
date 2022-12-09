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

	struct Colorable {
		Vector4	color = Color::WHITE;
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
}

struct FogEffect: Effect, Limitable, Colorable, Variable {};

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

struct MaskEffect: Effect, Imageable2D, Transformable2D, Channelable, Invertible {
	bool relative = false;
};

struct WaveEffect: Effect, Tuneable2D {};

struct BufferMaterial {
	Vector4
		color	= Color::WHITE,
		accent	= Color::NONE;
	Vector2			uvShift;
	MaskEffect		mask;
	WaveEffect		wave;
	GradientEffect	gradient;
};

struct AmbientEffect: Colorable, Variable {};

struct WorldMaterial {
	FogEffect		nearFog;
	FogEffect		farFog;
	AmbientEffect	ambient;
};

#define $mat RenderData::Material::
