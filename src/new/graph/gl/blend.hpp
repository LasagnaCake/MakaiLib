#ifndef MAKAILIB_GRAPH_BLEND_H
#define MAKAILIB_GRAPH_BLEND_H

namespace Makai::Graph {
	enum class BlendFunction {
		BF_ZERO,
		BF_ONE,
		BF_SRC_COLOR,
		BF_ONE_MINUS_SRC_COLOR,
		BF_DST_COLOR,
		BF_ONE_MINUS_DST_COLOR,
		BF_SRC_ALPHA,
		BF_ONE_MINUS_SRC_ALPHA,
		BF_DST_ALPHA,
		BF_ONE_MINUS_DST_ALPHA,
		BF_SRC_ALPHA_SATURATE,
		BF_CONST_COLOR,
		BF_ONE_MINUS_CONST_COLOR,
		BF_CONST_ALPHA,
		BF_ONE_MINUS_CONST_ALPHA,
		BF_ALPHA_SATURATE
	};

	enum class BlendEquation {
		BE_ZERO,
		BE_ONE,
		BE_ADD,
		BE_SUB,
		BE_REVERSE_SUB,
		BE_MIN,
		BE_MAX
	};

	struct BlendData {
		struct BlendFunctionData {
			BlendFunction
				srcColor = BlendFunction::BF_SRC_ALPHA,
				dstColor = BlendFunction::BF_ONE_MINUS_SRC_ALPHA,
				srcAlpha = BlendFunction::BF_ZERO,
				dstAlpha = BlendFunction::BF_ONE
			;
		} func = {};
		struct BlendEquationData {
			BlendEquation
				color = BlendFunction::BE_ADD,
				alpha = BlendFunction::BE_ADD
			;
		} eq = {};
	};

	struct Blendable {
		constexpr Blendable& setBlendFunction(
			BlendFunction const& srcColor,
			BlendFunction const& dstColor,
			BlendFunction const& srcAlpha,
			BlendFunction const& dstAlpha
		);

		constexpr Blendable& setBlendFunction(
			BlendFunction const& src,
			BlendFunction const& dst
		);

		constexpr Blendable& setBlendEquation(
			BlendEquation const& color,
			BlendEquation const& alpha
		);

		constexpr Blendable& setBlendEquation(
			BlendEquation const& eq
		);

		BlendData blend;

	protected:
		Blendable& setBlendMode(usize const& drawBuffer = 0);
	};
}

#endif // MAKAILIB_GRAPH_BLEND_H
