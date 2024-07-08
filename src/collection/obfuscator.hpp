#ifndef STRING_OBFUSCATION_H
#define STRING_OBFUSCATION_H

#include "helper.hpp"

template<typename TData>
struct Obfuscator {
	typedef TData DataType;

	constexpr virtual ~Obfuscator() {}

	virtual DataType deobfuscated() const = 0;

	DataType operator()() const	{return deobfuscated();}
	DataType operator*() const	{return deobfuscated();}
	operator DataType() const 	{return deobfuscated();}
};

template <usize N>
struct ObfuscatedString: Obfuscator<String> {
	constexpr static usize STRING_SIZE = N+1;
	typedef std::array<uint8, STRING_SIZE> ContainerType;

	constexpr ObfuscatedString(Decay::AsType<const char[STRING_SIZE]> const& str): data(obfuscate(str)) {}

	constexpr virtual ~ObfuscatedString() {}

	DataType deobfuscated() const final {
		DataType result;
		uint8 off = 0;
		for(uint8 c : data)
			result.push_back(off += c);
		return result;
	}

private:
	constexpr static ContainerType obfuscate(Decay::AsType<const char[STRING_SIZE]> const& str) {
		ContainerType result{};
		uint8 off = 0;
		for (usize i = 0; i < STRING_SIZE; ++i) {
			result[i] = (str[i] - off);
			off = str[i];
		}
		return result;
	}

	ContainerType data;
};

#endif // STRING_OBFUSCATION_H
