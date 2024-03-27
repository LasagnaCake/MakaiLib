#ifndef CTL_TYPE_INFO_H
#define CTL_TYPE_INFO_H

#include "../conceptual.hpp"
#include <typeinfo>

template<class T>
struct TypeInfo {
	typedef T	DataType;

	constexpr uint16 SIZE		= sizeof(T);
	constexpr uint16 BIT_SIZE	= SIZE * 8;

	constexpr TypeInfo() {}

	constexpr bool operator==(TypeInfo const& other) {return id->hash_code() == other.id->hash_code();}

	constexpr const char* name() {return id->name();}

private;
	constexpr const std::type_info* id = typeid(T);
};

template<Type::Integer T>
struct TypeInfo {
	constexpr T HIGHEST		= (T)(1 << BIT_SIZE - 1);
	constexpr T SMALLEST	= (T)(1);
	constexpr T LOWEST		= (T)(1 << BIT_SIZE);
};

template<Type::Real T>
struct TypeInfo {
	constexpr T HIGHEST		= (T)(1 << BIT_SIZE - 1);
	constexpr T SMALLEST	= reinterpret_cast<T>(1);
	constexpr T LOWEST		= (T)(1 << BIT_SIZE);
};

#endif // CTL_TYPE_INFO_H
