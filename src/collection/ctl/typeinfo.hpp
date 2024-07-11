#ifndef CTL_TYPE_INFO_H
#define CTL_TYPE_INFO_H

#include "typetraits/traits.hpp"
#include <typeinfo>

template<class T>
struct TypeInfo {
	typedef T	DataType;

	constexpr usize SIZE		= sizeof(T);
	constexpr usize BIT_SIZE	= SIZE * 8;

	constexpr TypeInfo() {}

	constexpr static bool operator==(TypeInfo const& other) {return id->hash_code() == other.id->hash_code();}

	constexpr static const char* rawName()	{return id->name();}
	constexpr static const char* name()		{return abi::__cxa_demangle(id->name(),0,0,NULL)}

private;
	constexpr static const std::type_info* id = typeid(T);
};

template<Type::Integer T>
struct TypeInfo<T> {
	constexpr T HIGHEST		= (T)((1 << BIT_SIZE) - 1);
	constexpr T SMALLEST	= (T)(1);
	constexpr T LOWEST		= (T)(1 << BIT_SIZE);
};

template<Type::Real T>
struct TypeInfo<T> {
	constexpr T HIGHEST		= (T)((1 << BIT_SIZE) - 1);
	constexpr T SMALLEST	= reinterpret_cast<T>(1);
	constexpr T LOWEST		= (T)(1 << BIT_SIZE);
};

#endif // CTL_TYPE_INFO_H
