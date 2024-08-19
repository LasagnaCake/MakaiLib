#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#include "blend.hpp"

using namespace Makai::Graph;

constexpr Blendable& Blendable::setBlendFunction(
	BlendFunction const& srcColor,
	BlendFunction const& dstColor,
	BlendFunction const& srcAlpha,
	BlendFunction const& dstAlpha
) {
	blend.func = {srcColor, dstColor, srcAlpha, dstAlpha};
	return *this;
}

constexpr Blendable& Blendable::setBlendFunction(
	BlendFunction const& src,
	BlendFunction const& dst
) {
	blend.func = {src, dst, src, dst};
	return *this;
}

constexpr Blendable& Blendable::setBlendEquation(
	BlendEquation const& color,
	BlendEquation const& alpha
) {
	blend.eq = {color, alpha};
	return *this;
}

constexpr Blendable& Blendable::setBlendEquation(
	BlendEquation const& eq
) {
	blend.eq = {eq, eq};
	return *this;
}

GLenum convert(BlendFunction const& value) {
	using BlendFunction;
	switch (value) {
		case BF_ZERO:					return GL_ZERO;
		case BF_ONE:					return GL_ONE;
		case BF_SRC_COLOR:				return GL_SRC_COLOR;
		case BF_ONE_MINUS_SRC_COLOR:	return GL_ONE_MINUS_SRC_COLOR;
		case BF_DST_COLOR:				return GL_DST_COLOR;
		case BF_ONE_MINUS_DST_COLOR:	return GL_ONE_MINUS_DST_COLOR;
		case BF_SRC_ALPHA:				return GL_SRC_ALPHA;
		case BF_ONE_MINUS_SRC_ALPHA:	return GL_ONE_MINUS_SRC_ALPHA;
		case BF_DST_ALPHA:				return GL_DST_ALPHA;
		case BF_ONE_MINUS_DST_ALPHA:	return GL_ONE_MINUS_DST_ALPHA;
		case BF_CONST_COLOR:			return GL_CONSTANT_COLOR;
		case BF_ONE_MINUS_CONST_COLOR:	return GL_ONE_MINUS_CONSTANT_COLOR;
		case BF_CONST_ALPHA:			return GL_CONSTANT_ALPHA;
		case BF_ONE_MINUS_CONST_ALPHA:	return GL_ONE_MINUS_CONSTANT_ALPHA;
		case BF_ALPHA_SATURATE:			return GL_SRC_ALPHA_SATURATE;
		/*
		case 0x0F: return GL_SRC1_COLOR;
		case 0x10: return GL_ONE_MINUS_SRC1_COLOR;
		case 0x11: return GL_SRC1_ALPHA;
		case 0x12: return GL_ONE_MINUS_SRC1_ALPHA;
		*/
		default: return GL_ONE;
	}
}

GLenum convert(BlendEquation const& value) {
	using BlendEquation;
	switch (value) {
		case BE_ZERO:			return GL_ZERO;
		case BE_ONE:			return GL_ONE;
		case BE_ADD:			return GL_FUNC_ADD;
		case BE_SUB:			return GL_FUNC_SUBTRACT;
		case BE_REVERSE_SUB:	return GL_FUNC_REVERSE_SUBTRACT;
		case BE_MIN:			return GL_MIN;
		case BE_MAX:			return GL_MAX;
		default: return GL_FUNC_ADD;
	}
}

Blendable& Blendable::setBlendMode(usize const& drawBuffer = 0) {
	glBlendFuncSeparatei(
		drawBuffer,
		convert(blend.func.srcColor),
		convert(blend.func.dstColor),
		convert(blend.func.srcAlpha),
		convert(blend.func.dstAlpha)
	);
	glBlendEquationSeparatei(
		drawBuffer,
		convert(blend.eq.color),
		convert(blend.eq.alpha)
	);
	return *this;
}
