#include <cppcodec/base64_rfc4648.hpp>
#include <cppcodec/base32_rfc4648.hpp>

#include "encdec.hpp"

using namespace Makai;

using namespace Data;

BinaryData Data::decode(String const& data, EncodingType const& encoding) try {
	switch (encoding) {
		case EncodingType::ET_BASE32: return cppcodec::base32_rfc4648::decode(data);
		case EncodingType::ET_BASE64: return cppcodec::base64_rfc4648::decode(data);
		default: throw Error::InvalidValue(
			"Invalid encoding ype!",
			__FILE__,
			toString(__LINE__),
			"decode"
		);
	}
} catch (cppcodec::parse_error const& e) {
	throw Error::FailedAction(
		"Failed at decoding byte data!",
		__FILE__,
		toString(__LINE__),
		"decode",
		e.what()
	);
}

String Data::encode(BinaryData const& data, EncodingType const& encoding) try {
	switch (encoding) {
		case EncodingType::ET_BASE32: return cppcodec::base32_rfc4648::encode(data);
		case EncodingType::ET_BASE64: return cppcodec::base64_rfc4648::encode(data);
		default: throw Error::InvalidValue(
			"Invalid encoding ype!",
			__FILE__,
			toString(__LINE__),
			"decode"
		);
	}
} catch (cppcodec::parse_error const& e) {
	throw Error::FailedAction(
		"Failed at decoding byte data!",
		__FILE__,
		toString(__LINE__),
		"decode",
		e.what()
	);
}
