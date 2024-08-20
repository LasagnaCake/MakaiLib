#include <stringstream>

#include "color.hpp"

using namespace Makai::Graph;

using namespace Color;

// TODO: Optimize this
constexpr Vector4 Color::hueToPastel(float hue) {
	hue *= PI;
	Vector3 res(
		cos(hue),
		cos(hue + TAU * (1.0/3.0)),
		cos(hue + TAU * (2.0/3.0))
	);
	res = (res^2.0).normalized() * SQRT2;
	return Vector4(res.clamped(0, 1), 1);
}

constexpr Vector4 Color::hueToRGB(float h) {
	h -= floor(h);
	return Vector4(
		_hueify(h + 1.0/3.0),
		_hueify(h),
		_hueify(h - 1.0/3.0),
		1
	);
}

// Custom transparency level
constexpr Vector4 Color::alpha(float level) {
	return Vector4(1, 1, 1, Math::clamp(level, -0.1f, 1.0f));
}

// Custom color
constexpr Vector4 Color::fromRGBA(float r, float g, float b, float a = 1) {
	return Vector4(r, g, b, a);
}

// Custom color
constexpr Vector4 Color::fromRGBA255(float r, float g, float b, float a = 255) {
	return Vector4(r, g, b, a) / 255;
}

constexpr Vector4 Color::fromGrayscale(float gray, float a = 1) {
	return Vector4(gray, gray, gray, a);
}

constexpr Vector4 Color::fromGrayscale255(float gray, float a = 255) {
	return fromGrayscale(gray, a) / 255;
}

constexpr Vector4 Color::fromHSL(float h, float s, float l, float a) {
	Vector4 res = hueToRGB(h);
	res *= (l * 2);
	Vector4 gray(Vector3((res.x + res.y + res.z) / 3), 1);
	res = Math::lerp(gray, res, Vector4(s));
	res.w = a;
	return res.clamped(0, 1);
}

constexpr Vector4 Color::fromHexCodeRGBA(uint32 code) {
	uint8
		r = (code >> 24)	& 0xFF,
		g = (code >> 16)	& 0xFF,
		b = (code >> 8)		& 0xFF,
		a = (code)			& 0xFF
	;
	return Vector4(r, g, b, a) / 255;
}

constexpr Vector4 Color::fromHexCodeRGB(uint32 code) {
	uint8
		r = (code >> 16)	& 0xFF,
		g = (code >> 8)		& 0xFF,
		b =	(code)			& 0xFF
	;
	return Vector4(r, g, b, 255) / 255;
}

Vector4 Color::fromHexCodeString(String code) {
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

constexpr uint32 Color::toHexCodeRGB(uint32 const& color) {
	return color >> 8;
}

constexpr uint32 Color::toHexCodeRGBA(uint32 const& color) {
	return (color << 8) | 0xFF;
}

constexpr uint32 Color::toHexCodeRGBA(Vector4 const& color) {
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

constexpr uint32 Color::toHexCodeRGB(Vector4 const& color) {
	return toHexCodeRGBA(color) >> 8;
}

constexpr String Color::toHexCodeString(
	Vector4 const& color,
	bool const& toRGB		= false,
	bool const& webColor	= true
) {
	String code;
	code += (webColor ? "#" : "0x");
	uint32 hci = (toRGB ? toHexCodeRGB(color) : toHexCodeRGBA(color));
	const uint8 hclen{toRGB ? 6 : 8};
	uint8 nib{};
	for SRANGE(i, 0, hclen) {
		nib = hci >> ((hclen - 1) - i) * 4;
		code += (char)(
			((nib & 0xF) < 0xA)
		?	((nib & 0xF) + 0x30)
		:	((nib & 0xF) - 0xA + 0x41)
		);
	}
	return code;
}

constexpr Vector4 Color::fromJSON(JSON::JSONData const& json) {
	try {
		if (json.isArray())
			return Vector4(
				json[0].value<float>(0),
				json[1].value<float>(0),
				json[2].value<float>(0),
				json[3].value<float>(1)
			);
		else if (json.isString())
			return Color::fromHexCodeString(json.get<String>());
		else if (json.isNumber())
			return Vector4(json.value<float>(1));
	} catch (...) {
		return Vector4(1);
	}
}
