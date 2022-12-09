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

	struct Imageable {
		Texture2D* image;
	};

	struct Channelable {
		int channel = -1;
	};

	struct Transformable {
		Transform2D trans;
	};

	struct Invertible {
		bool invert = false;
	};
}

namespace {
	using namespace Module;
}

struct FogEffect: Effect, Limitable, Variable, Colorable {};

struct TextureEffect: Effect, Imageable {
	float alphaClip = 0.1;
};

struct WarpEffect: Effect, Imageable, Transformable {
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

struct MaskEffect: Effect, Imageable, Transformable, Channelable, Invertible {
	bool relative = false;
};

struct WaveEffect: Effect {
	Vector2
		amplitude	= Vector2(0),
		frequency	= Vector2(0),
		shift		= Vector2(0);
};

struct BufferMaterial {
	MaskEffect		mask;
	WaveEffect		wave;
	GradientEffect	gradient;
};

struct AmbientEffect: Colorable, Variable {};

struct WorldMaterial {
	FogEffect		fog;
	FogEffect		mist;
	AmbientEffect	ambient;
};

#define $dat RenderData::Data
