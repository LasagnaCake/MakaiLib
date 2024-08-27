#include <cryptopp/sha3.h>

#include "hash.hpp"

using Makai::Data::HashMode;

template<class T>
List<ubyte> hashData(List<ubyte> const& data) {
	List<ubyte> result;
	T hasher;
	hasher.Update((const byte*)data.data(), data.size());
	result.resize(hasher.DigestSize());
	hasher.Final((byte*)result.data());
	return result;
}

List<ubyte> Makai::Data::hashed(List<ubyte> const& data, Makai::Data::HashMode const& mode) {
	switch (mode) {
		case HashMode::HM_SHA3_224: return hashData<CryptoPP::SHA3_224>(data);
		case HashMode::HM_SHA3_256: return hashData<CryptoPP::SHA3_256>(data);
		case HashMode::HM_SHA3_384: return hashData<CryptoPP::SHA3_384>(data);
		case HashMode::HM_SHA3_512: return hashData<CryptoPP::SHA3_512>(data);
	};
	return data;
}

void Makai::Data::hash(List<ubyte>& data, Makai::Data::HashMode const& mode) {
	data = Makai::Data::hashed(data, mode);
}
