#ifndef MAKAILIB_GRAPH_BLEND_H
#define MAKAILIB_GRAPH_BLEND_H

#include "../../ctl/ctl.hpp"

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

	struct BlendMode {
		struct FunctionOption {
			BlendFunction
				srcColor = BlendFunction::BF_SRC_ALPHA,
				dstColor = BlendFunction::BF_ONE_MINUS_SRC_ALPHA,
				srcAlpha = BlendFunction::BF_ZERO,
				dstAlpha = BlendFunction::BF_ONE
			;
		} func = {};
		struct EquationOption {
			BlendEquation
				color = BlendEquation::BE_ADD,
				alpha = BlendEquation::BE_ADD
			;
		} eq = {};
	};

	struct Blendable {
		constexpr Blendable& setBlendFunction(
			BlendFunction const& srcColor,
			BlendFunction const& dstColor,
			BlendFunction const& srcAlpha,
			BlendFunction const& dstAlpha
		) {
			blend.func = {srcColor, dstColor, srcAlpha, dstAlpha};
			return *this;
		}

		constexpr Blendable& setBlendFunction(
			BlendFunction const& src,
			BlendFunction const& dst
		) {
			blend.func = {src, dst, src, dst};
			return *this;
		}

		constexpr Blendable& setBlendEquation(
			BlendEquation const& color,
			BlendEquation const& alpha
		) {
			blend.eq = {color, alpha};
			return *this;
		}

		constexpr Blendable& setBlendEquation(
			BlendEquation const& eq
		) {
			blend.eq = {eq, eq};
			return *this;
		}

		BlendMode blend;

	protected:
		void useBlendMode(usize const& drawBuffer = 0) const;
	};
}

#endif // MAKAILIB_GRAPH_BLEND_H
