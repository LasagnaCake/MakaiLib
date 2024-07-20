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

template<Type::Number T>
struct TypeInfo<T> {
	constexpr T HIGHEST		= bitcast<T>(usize(0) - 1);
	constexpr T SMALLEST	= bitcast<T>(1);
	constexpr T LOWEST		= bitcast<T>(0);
};

#endif // CTL_TYPE_INFO_H
