#ifndef MAKAILIB_GRAPH_MATERIAL_EFFECT_H
#define MAKAILIB_GRAPH_MATERIAL_EFFECT_H

namespace Makai::Graph::Material {
	namespace Effect {
		namespace Base {
			struct Toggleable {
				bool enabled = false;
			};

			struct Limitable {
				float
					start	= 0.0,
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
				Vector3	color = 1;
			};

			struct Imageable2D {
				Texture2D image = nullptr;
			};

			struct Channelable {
				int channel = -1;
			};

			struct Transformable2D {
				Transform2D trans;
			};

			struct Positionable2D {
				Vector2 position;
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

			struct Countable {
				size_t count = 1;
			};
		}

		namespace {
			using namespace Base;
		}

		enum class EffectBlendFunction: unsigned int {
			EBF_ZERO = 0,
			EBF_ONE,
			EBF_SRC,
			EBF_ONE_MINUS_SRC,
			EBF_DST,
			EBF_ONE_MINUS_DST,
		};

		enum class EffectBlendEquation: unsigned int {
			EBE_ADD,
			EBE_SUBTRACT,
			EBE_MULTIPLY,
			EBE_DIVIDE,
			EBE_REVERSE_SUBTRACT,
			EBE_REVERSE_DIVIDE,
			EBE_ADD_CLAMP,
			EBE_SUBTRACT_CLAMP,
			EBE_MULTIPLY_CLAMP,
			EBE_DIVIDE_CLAMP,
			EBE_REVERSE_SUBTRACT_CLAMP,
			EBE_REVERSE_DIVIDE_CLAMP,
			EBE_MAX,
			EBE_MIN,
		};

		enum class EffectBlendSource: unsigned int {
			EBS_ZERO,
			EBS_ONE,
			EBS_COLOR,
			EBS_ALPHA,
			EBS_NOISE,
			EBS_SOLID_COLOR,
			EBS_SOLID_ALPHA,
		};

		struct EffectBlendMode {
			EffectBlendFunction source		= EffectBlendFunction::EBF_SRC;
			EffectBlendFunction destination	= EffectBlendFunction::EBF_DST;
			EffectBlendEquation equation	= EffectBlendEquation::EBE_MULTIPLY;
		};

		struct EffectBlendSetting {
			EffectBlendMode	color	= {EffectBlendFunction::EBF_ONE, EffectBlendFunction::EBF_DST, EffectBlendEquation::EBE_MULTIPLY};
			EffectBlendMode	alpha	= {EffectBlendFunction::EBF_SRC, EffectBlendFunction::EBF_ONE, EffectBlendEquation::EBE_MULTIPLY};
		};

		struct Gradient: Toggleable, Channelable, Invertible {
			Vector4
				begin	= Color::BLACK,
				end		= Color::WHITE;
		};

		struct Negative: Toggleable, Variable {};

		struct Image: Toggleable, Imageable2D {};

		struct Warp: Image, Transformable2D {
			unsigned int
				channelX = 0,
				channelY = 1;
		};

		// Object Material Effects

		struct Texture: Image {
			float alphaClip = 0.1;
		};

		struct Emission: Image, Variable {
		};

		struct NormalMap: Image, Variable {
		};

		// Buffer Material Effects

		struct Mask: Toggleable, Imageable2D, Transformable2D, Invertible {
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

		struct Wave: Toggleable, Tuneable2D {
			WaveShape	shape	= WS_SQUARE;
		};

		struct Rainbow: Toggleable, Variable {
			Vector2 frequency		= 0.0;
			Vector2 shift			= 0.0;
			bool	absoluteColor	= false;
			bool	polar			= false;
			float	polarShift		= 0.0;
		};

		struct Blur: Toggleable, Variable2D {};

		struct Outline: Toggleable, Sizeable2D, ColorableRGBA {
			bool relativeAlpha = true;
		};

		struct PolarWarp: Toggleable, Sizeable, Positionable2D, Variable2D, ColorableRGBA {
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

		struct Noise: Toggleable, Variable, Transformable2D {
			float			seed	= 1;
			NoiseType		type	= NoiseType::NT_NOISE_SUPER;
			NoiseBlendMode	blend;
		};

		// World Material Effects

		struct Fog: Toggleable, Limitable, ColorableRGBA, Variable {};

		struct Ambient: ColorableRGB, Variable {};
	}
}

namespace MkEffect = Makai::Graph::Material::Effect;

#endif // MAKAILIB_GRAPH_MATERIAL_EFFECT_H
