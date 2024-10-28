#ifndef CTL_TYPE_INFO_H
#define CTL_TYPE_INFO_H

#include "typetraits/traits.hpp"
#include "meta/logic.hpp"
#include "meta/enumtype.hpp"
#include "namespace.hpp"
#include <typeinfo>
#include <cxxabi.h>

CTL_NAMESPACE_BEGIN

namespace Base {
	template<class T>
	struct BasicInfo {
		typedef T	DataType;

		constexpr static usize SIZE		= sizeof(T);
		constexpr static usize BIT_SIZE	= SIZE * 8;

		constexpr BasicInfo() {}
		constexpr BasicInfo(BasicInfo const& other)	= delete;
		constexpr BasicInfo(BasicInfo&& other)		= delete;

		constexpr bool operator==(BasicInfo const& other) {return id->hash_code() == other.id->hash_code();}

		constexpr static const char* rawName()	{return id->name();}
		constexpr static const char* name()		{return abi::__cxa_demangle(id->name(),0,0,NULL);}

	private:
		constexpr static const std::type_info* id = &typeid(T);
	};
}

template<class T, auto H, auto S, auto L>
struct ValueLimit {
	constexpr static T HIGHEST	= H;
	constexpr static T SMALLEST	= S;
	constexpr static T LOWEST	= L;
};

template<Type::Number T>
struct NumberLimit;

template<> struct NumberLimit<bool>:	ValueLimit<bool,	0x1,				1,	0					>	{};
template<> struct NumberLimit<char>:	ValueLimit<char,	0x7F,				1,	-0x80				>	{};

template<> struct NumberLimit<int8>:	ValueLimit<int8,	0x7F,				1,	-0x80				>	{};
template<> struct NumberLimit<int16>:	ValueLimit<int16,	0x7FFF,				1,	-0x8000				>	{};
template<> struct NumberLimit<int32>:	ValueLimit<int32,	0x7FFFFFFF,			1,	-0x80000000			>	{};
template<> struct NumberLimit<int64>:	ValueLimit<int64,	0x7FFFFFFFFFFFFFFF,	1,	-0x8000000000000000	>	{};

template<> struct NumberLimit<uint8>:	ValueLimit<uint8,	0xFF,				1,	0					>	{};
template<> struct NumberLimit<uint16>:	ValueLimit<uint16,	0xFFFF,				1,	0					>	{};
template<> struct NumberLimit<uint32>:	ValueLimit<uint32,	0xFFFFFFFF,			1,	0					>	{};
template<> struct NumberLimit<uint64>:	ValueLimit<uint64,	0xFFFFFFFFFFFFFFFF,	1,	0					>	{};

template<> struct NumberLimit<int>:		NumberLimit<Meta::DualType<sizeof(int) == sizeof(int32),	int32,	int16>>		{};
template<> struct NumberLimit<uint>:	NumberLimit<Meta::DualType<sizeof(uint) == sizeof(uint32),	uint32,	uint16>>	{};

template<typename T>
struct TypeInfo;

template<Type::Integer T>
struct TypeInfo<T>: NumberLimit<T>, Base::BasicInfo<T> {};

template<Type::Float T>
struct TypeInfo<T>: Base::BasicInfo<T> {};

template<Type::Enumerator T>
struct TypeInfo<T>: NumberLimit<Meta::EnumType<T>>, Base::BasicInfo<T> {};

template<Type::Class T>
struct TypeInfo<T>: Base::BasicInfo<T> {};

CTL_NAMESPACE_END

#endif // CTL_TYPE_INFO_H