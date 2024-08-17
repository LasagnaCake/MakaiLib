#ifndef CTL_TYPE_INFO_H
#define CTL_TYPE_INFO_H

#include "typetraits/traits.hpp"
#include <typeinfo>

template<class T>
struct TypeInfo {
	typedef T	DataType;

	constexpr static usize SIZE		= sizeof(T);
	constexpr static usize BIT_SIZE	= SIZE * 8;

	constexpr TypeInfo() {}

	constexpr bool operator==(TypeInfo const& other) {return id->hash_code() == other.id->hash_code();}

	constexpr static const char* rawName()	{return id->name();}
	constexpr static const char* name()		{return abi::__cxa_demangle(id->name(),0,0,NULL);}

private:
	constexpr static const std::type_info* id = typeid(T);
};

template<class T, auto H, auto S, auto L>
struct ValueLimit {
	constexpr static T HIGHEST	= H;
	constexpr static T SMALLEST	= S;
	constexpr static T LOWEST	= L;
};

template<Type::Number T>
struct NumberLimit;

template<> struct NumberLimit<int8>:	ValueLimit<int8,	0x7F,				1,	-0x80				>	{};
template<> struct NumberLimit<int16>:	ValueLimit<int16,	0x7FFF,				1,	-0x8000				>	{};
template<> struct NumberLimit<int32>:	ValueLimit<int32,	0x7FFFFFFF,			1,	-0x80000000			>	{};
template<> struct NumberLimit<int64>:	ValueLimit<int64,	0x7FFFFFFFFFFFFFFF,	1,	-0x8000000000000000	>	{};

template<> struct NumberLimit<uint8>:	ValueLimit<uint8,	0xFF,				1,	0					>	{};
template<> struct NumberLimit<uint16>:	ValueLimit<uint16,	0xFFFF,				1,	0					>	{};
template<> struct NumberLimit<uint32>:	ValueLimit<uint32,	0xFFFFFFFF,			1,	0					>	{};
template<> struct NumberLimit<uint64>:	ValueLimit<uint64,	0xFFFFFFFFFFFFFFFF,	1,	0					>	{};

template<Type::Integer T>
struct TypeInfo<T>: NumberLimit<T> {};
#endif // CTL_TYPE_INFO_H
