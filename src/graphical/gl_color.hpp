#ifndef VECTOR_COLOR_SPACE_H
#define VECTOR_COLOR_SPACE_H

#include "../anchors.hpp"

namespace Color {
	namespace {
		using namespace Vector;

		float _hueify(float t) {
			if(t < 0) t++;
            if(t > 1) t--;
            if(t < 1.0/6.0) return 6.0 * t;
            if(t < 1.0/2.0) return 1;
            if(t < 2.0/3.0) return (2.0/3.0 - t) * 6.0;
            return 0;
		}
	}
	typedef Vector4 Color;

	// Custom transparency level
	inline Vector4 alpha(float level) {
		return Vector4(1, 1, 1, Math::clamp(level, -0.1f, 1.0f));
	}

	// Custom color
	inline Vector4 fromRGBA255(float r, float g, float b, float a = 255) {
		return Vector4(r, g, b, a) / 255;
	}

	inline Vector4 fromGrayscale(float gray, float a = 1) {
		return Vector4(gray, gray, gray, a);
	}

	inline Vector4 fromGrayscale255(float gray, float a = 255) {
		return fromGrayscale(gray, a) / 255;
	}

	inline Vector4 fromHexCodeRGBA(uint32 code) {
		uint8
			r = (code >> 24)	& 0xFF,
			g = (code >> 16)	& 0xFF,
			b = (code >> 8)		& 0xFF,
			a = (code)			& 0xFF
		;
		return Vector4(r, g, b, a) / 255;
	}

	inline Vector4 fromHexCodeRGB(uint32 code) {
		uint8
			r = (code >> 16)	& 0xFF,
			g = (code >> 8)		& 0xFF,
			b =	(code)			& 0xFF
		;
		return Vector4(r, g, b, 255) / 255;
	}

	inline Vector4 fromHexCodeString(String code) {
		// Remove hex markers if necessary
		code = regexReplace(code, "(#|0x)", "");
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

	inline uint32 toHexCodeRGBA(Vector4 const& color) {
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

	inline uint32 toHexCodeRGB(Vector4 const& color) {
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

	// TODO: Optimize this
	Vector4 hueToPastel(float hue) {
		hue *= PI;
		Vector3 res(
			cos(hue),
			cos(hue + TAU * (1.0/3.0)),
			cos(hue + TAU * (2.0/3.0))
		);
		res = (res^2.0).normalized() * SQRT2;
		return Vector4(res.clamped(0, 1), 1);
	}

	Vector4 hueToRGB(float h) {
		h -= floor(h);
        return Vector4(
			_hueify(h + 1.0/3.0),
			_hueify(h),
			_hueify(h - 1.0/3.0),
			1
		);
	}

	Vector4 fromHSL(float h, float s, float l, float a) {
		Vector4 res = hueToRGB(h);
		res *= (l * 2);
		Vector4 gray(Vector3((res.x + res.y + res.z) / 3), 1);
		res = Math::lerp(gray, res, Vector4(s));
		res.w = a;
		return res.clamped(0, 1);
	}

	// Transparency
	const Vector4 NONE			= Vector4(0);
	const Vector4 CLEAR			= alpha(.0);
	const Vector4 SEMIMISTY		= alpha(.125);
	const Vector4 MISTY			= alpha(.25);
	const Vector4 SEMILUCENT	= alpha(.375);
	const Vector4 LUCENT		= alpha(.50);
	const Vector4 SEMIMILKY		= alpha(.625);
	const Vector4 MILKY			= alpha(.75);
	const Vector4 SEMISOLID		= alpha(.875);
	const Vector4 SOLID			= alpha(1);
	// Luminance
	const Vector4 WHITE			= fromGrayscale(1, 1);
	const Vector4 LIGHTGRAY		= fromGrayscale(.75, 1);
	const Vector4 GRAY			= fromGrayscale(.5, 1);
	const Vector4 DARKGRAY		= fromGrayscale(.25, 1);
	const Vector4 BLACK			= fromGrayscale(0, 1);
	// Primary Colors
	const Vector4 RED			= Vector4(1,	0,	0);
	const Vector4 GREEN			= Vector4(0,	1,	0);
	const Vector4 BLUE			= Vector4(0,	0,	1);
	// Secondary Colors
	const Vector4 YELLOW		= Vector4(1,	1,	0);
	const Vector4 MAGENTA		= Vector4(1,	0,	1);
	const Vector4 CYAN			= Vector4(0,	1,	1);
	// Tertiary Colors
	const Vector4 ORANGE		= Vector4(1,	.5,	0);
	const Vector4 AZURE			= Vector4(0,	.5,	1);
	const Vector4 TEAL			= Vector4(0,	1,	.5);
	const Vector4 LIME			= Vector4(.5,	1,	0);
	const Vector4 PURPLE		= Vector4(.5,	0,	1);
	const Vector4 PINK			= Vector4(1,	0,	.5);

	const Vector4 rainbow8[] = {
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

	const Vector4 rainbow[] = {
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
