#ifndef VECTOR_COLOR_SPACE_H
#define VECTOR_COLOR_SPACE_H

#include "../anchors.hpp"

namespace Color {
	namespace {
		constexpr float _hueify(float t) {
			if(t < 0) t++;
            if(t > 1) t--;
            if(t < 1.0/6.0) return 6.0 * t;
            if(t < 1.0/2.0) return 1;
            if(t < 2.0/3.0) return (2.0/3.0 - t) * 6.0;
            return 0;
		}
	}
	typedef Vector4 Color;

	// TODO: Optimize this
	constexpr Vector4 hueToPastel(float hue) {
		hue *= PI;
		Vector3 res(
			cos(hue),
			cos(hue + TAU * (1.0/3.0)),
			cos(hue + TAU * (2.0/3.0))
		);
		res = (res^2.0).normalized() * SQRT2;
		return Vector4(res.clamped(0, 1), 1);
	}

	constexpr Vector4 hueToRGB(float h) {
		h -= floor(h);
        return Vector4(
			_hueify(h + 1.0/3.0),
			_hueify(h),
			_hueify(h - 1.0/3.0),
			1
		);
	}

	// Custom transparency level
	constexpr Vector4 alpha(float level) {
		return Vector4(1, 1, 1, Math::clamp(level, -0.1f, 1.0f));
	}

	// Custom color
	constexpr Vector4 fromRGBA(float r, float g, float b, float a = 1) {
		return Vector4(r, g, b, a);
	}

	// Custom color
	constexpr Vector4 fromRGBA255(float r, float g, float b, float a = 255) {
		return Vector4(r, g, b, a) / 255;
	}

	constexpr Vector4 fromGrayscale(float gray, float a = 1) {
		return Vector4(gray, gray, gray, a);
	}

	constexpr Vector4 fromGrayscale255(float gray, float a = 255) {
		return fromGrayscale(gray, a) / 255;
	}

	constexpr Vector4 fromHSL(float h, float s, float l, float a) {
		Vector4 res = hueToRGB(h);
		res *= (l * 2);
		Vector4 gray(Vector3((res.x + res.y + res.z) / 3), 1);
		res = Math::lerp(gray, res, Vector4(s));
		res.w = a;
		return res.clamped(0, 1);
	}

	constexpr Vector4 fromHexCodeRGBA(uint32 code) {
		uint8
			r = (code >> 24)	& 0xFF,
			g = (code >> 16)	& 0xFF,
			b = (code >> 8)		& 0xFF,
			a = (code)			& 0xFF
		;
		return Vector4(r, g, b, a) / 255;
	}

	constexpr Vector4 fromHexCodeRGB(uint32 code) {
		uint8
			r = (code >> 16)	& 0xFF,
			g = (code >> 8)		& 0xFF,
			b =	(code)			& 0xFF
		;
		return Vector4(r, g, b, 255) / 255;
	}

	Vector4 fromHexCodeString(String code) {
		// Remove hex markers if necessary
		code = regexReplace(code, "(#|0x)", "");
		// Check if color exists
		if (code.empty())
			throw Error::InvalidValue(
				"Color value does not exist!",
				__FILE__,
				toString(__LINE__),
				"fromHexCodeString",
				"Make sure the color values are correct!"
			);
		// Check if color value is valid
		if (code.size() < 3 || code.size() > 8 || !Helper::isHexString(code))
			throw Error::InvalidValue(
				"Invalid color value \"#"+ code +"\"!",
				__FILE__,
				toString(__LINE__),
				"fromHexCodeString",
				"Make sure the color values are correct!"
			);
		// Bit fuckery for 4-bit color
		if (code.size() <= 4) {
			std::stringstream nc;
			nc
			<< code[0] << code[0]
			<< code[1] << code[1]
			<< code[2] << code[2]
			;
			if (code.size() == 4)
				nc << code[3] << code[3];
			code = nc.str();
		}
		// Convert to uint32
		if (code.size() == 6)
			return fromHexCodeRGB(toUInt32(code, 16));
		return fromHexCodeRGBA(toUInt32(code, 16));
	}

	constexpr inline uint32 toHexCodeRGBA(Vector4 const& color) {
		uint8
			r = Math::clamp(color.x, 0.0f, 1.0f) * 255,
			g = Math::clamp(color.y, 0.0f, 1.0f) * 255,
			b = Math::clamp(color.z, 0.0f, 1.0f) * 255,
			a = Math::clamp(color.w, 0.0f, 1.0f) * 255
		;
		uint32 code =
			(((uint32)r) << 24)
		|	(((uint32)g) << 16)
		|	(((uint32)b) << 8)
		|	((uint32)a)
		;
		return code;
	}

	constexpr inline uint32 toHexCodeRGB(Vector4 const& color) {
		return toHexCodeRGBA(color) >> 8;
	}

	inline String toHexCodeString(Vector4 const& color, bool toRGB = false, bool webColor = false) {
		std::stringstream code;
		code
		<<	(webColor ? "#" : "0x")
		<<	std::setfill('0')
		<<	std::setw(toRGB ? 6 : 8)
		<<	std::hex
		<<	(toRGB ? toHexCodeRGB(color) : toHexCodeRGBA(color))
		;
		return code.str();
	}

	// Transparency
	constexpr Vector4 NONE			= Vector4(0);
	constexpr Vector4 CLEAR			= alpha(.0);
	constexpr Vector4 SEMIMISTY		= alpha(.125);
	constexpr Vector4 MISTY			= alpha(.25);
	constexpr Vector4 SEMILUCENT	= alpha(.375);
	constexpr Vector4 LUCENT		= alpha(.50);
	constexpr Vector4 SEMIMILKY		= alpha(.625);
	constexpr Vector4 MILKY			= alpha(.75);
	constexpr Vector4 SEMISOLID		= alpha(.875);
	constexpr Vector4 SOLID			= alpha(1);
	// Luminance
	constexpr Vector4 WHITE			= fromGrayscale(1, 1);
	constexpr Vector4 LIGHTGRAY		= fromGrayscale(.75, 1);
	constexpr Vector4 GRAY			= fromGrayscale(.5, 1);
	constexpr Vector4 DARKGRAY		= fromGrayscale(.25, 1);
	constexpr Vector4 BLACK			= fromGrayscale(0, 1);
	// Primary Colors
	constexpr Vector4 RED			= fromRGBA(1,	0,	0);
	constexpr Vector4 GREEN			= fromRGBA(0,	1,	0);
	constexpr Vector4 BLUE			= fromRGBA(0,	0,	1);
	// Secondary Colors
	constexpr Vector4 YELLOW		= fromRGBA(1,	1,	0);
	constexpr Vector4 MAGENTA		= fromRGBA(1,	0,	1);
	constexpr Vector4 CYAN			= fromRGBA(0,	1,	1);
	// Tertiary Colors
	constexpr Vector4 ORANGE		= fromRGBA(1,	.5,	0);
	constexpr Vector4 AZURE			= fromRGBA(0,	.5,	1);
	constexpr Vector4 TEAL			= fromRGBA(0,	1,	.5);
	constexpr Vector4 LIME			= fromRGBA(.5,	1,	0);
	constexpr Vector4 PURPLE		= fromRGBA(.5,	0,	1);
	constexpr Vector4 PINK			= fromRGBA(1,	0,	.5);

	constexpr Vector4 rainbow8[] = {
		PURPLE,
		MAGENTA,
		RED,
		ORANGE,
		YELLOW,
		LIME,
		GREEN,
		CYAN,
		BLUE
	};

	constexpr Vector4 rainbow[] = {
		PURPLE,
		MAGENTA,
		PINK,
		RED,
		ORANGE,
		YELLOW,
		LIME,
		GREEN,
		TEAL,
		CYAN,
		AZURE,
		BLUE
	};
}

#endif
