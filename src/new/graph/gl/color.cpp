#include <stringstream>

#include "color.hpp"

using namespace Makai::Graph;

using namespace Color;

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

Vector4 Color::fromJSON(JSON::JSONData const& json) {
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
