#ifndef MAKAILIB_DATA_ENCDEC_H
#define MAKAILIB_DATA_ENCDEC_H

#include "../ctl/ctl.hpp"

namespace Makai::Data {
	enum class EncodingType {
		ET_UNKNOWN = -1,
		ET_BASE32,
		ET_BASE64
	};

	using BinaryData = List<ubyte>;

	BinaryData decode(String const& data, EncodingType const& type);

	String encode(BinaryData const& data, EncodingType const& type);

	String			toString(EncodingType const& type);
	EncodingType	fromString(String const& type);
}

#endif // MAKAILIB_DATA_ENCDEC_H
