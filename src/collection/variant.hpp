#ifndef VARIANT_TYPE_H
#define VARIANT_TYPE_H

#include <typeinfo>
#include <any>
#include "helper.hpp"
#include "dataview.hpp"
#include "ctl/ctypes.hpp"
#include "conceptual.hpp"

namespace Type {
	template <typename T>
	concept Variable = !(
		Reference<T>
	||	Void<T>
	||	Null<T>
	||	Array<T>
	||	Constant<T>
	);
};

struct Variant {
	constexpr Variant() {}

	template<Type::Variable T> constexpr Variant(T const& v)			{set(v);}
	template<Type::Variable T> constexpr Variant(Variant const& v)		{set(v.value, v.type);}
	template<Type::Variable T> constexpr Variant(Variant&& v)			{set(v.value, v.type);}

	template<Type::Variable T> inline Variant& operator=(T const& v)		{return set(v);}
	template<Type::Variable T> inline Variant& operator=(Variant const& v)	{return set(v.value, v.type);}
	template<Type::Variable T> inline Variant& operator=(Variant&& v)		{return set(v.value, v.type);}

	template<Type::Fundamental T> constexpr operator T() const									{return get<T>();}
	template<Type::Variable T> constexpr operator T() const requires (!Type::Fundamental<T>)	{return get<T>();}

	template<Type::Variable T>
	constexpr Variant& set(T const& v) {return set(v, &typeid(T));}

	#define FUNDAMENTAL_CASE(TYPE) case &typeid(TYPE): return T(as<TYPE>());
	template<Type::Fundamental T>
	constexpr T get() const {
		switch(type) {
			// Fundamental types
			FUNDAMENTAL_CASE(void);
			FUNDAMENTAL_CASE(bool);
			FUNDAMENTAL_CASE(short);
			FUNDAMENTAL_CASE(int);
			FUNDAMENTAL_CASE(long);
			FUNDAMENTAL_CASE(llong);
			FUNDAMENTAL_CASE(ushort);
			FUNDAMENTAL_CASE(uint);
			FUNDAMENTAL_CASE(ulong);
			FUNDAMENTAL_CASE(ullong);
			FUNDAMENTAL_CASE(char);
			FUNDAMENTAL_CASE(schar);
			FUNDAMENTAL_CASE(uchar);
			FUNDAMENTAL_CASE(float);
			FUNDAMENTAL_CASE(double);
			FUNDAMENTAL_CASE(ldouble);
			// Everything else
			default: throw Error::InvalidType(
				toString(
					"Type '",
					NAMEOF(typeid(T)),
					"' is not a fundamental type!\nInternal type: '",
					NAMEOF(*type),
					"'"
				),
				__FILE__,
				toString("get<", NAMEOF(typeid(T)), ">"),
				"Please ensure T is a fundamental type!"
			);
		}
	}
	#undef FUNDAMENTAL_CASE

	template<Type::Variable T> constexpr T get() const requires (!Type::Fundamental<T>) {return as<T>();}

private:
	typedef std::any AnyType;

	template<class T>
	constexpr T as() const {
		if (typeid(T) != *type)
			throw Error::InvalidType(
				toString(
					"Type '",
					NAMEOF(typeid(T)),
					"' does not match internal type '",
					NAMEOF(*type),
					"'!"
				),
				__FILE__,
				toString(__LINE__),
				toString("as<", NAMEOF(typeid(T)), ">"),
				"Please ensure types match!"
			);
		return std::any_cast<T>(value);
	}

	Variant& set(AnyType const& v, const std::type_info* const& t) {
		type	= t;
		value	= v;
		return *this;
	}

	const std::type_info*	type = &typeid(void);
	AnyType					value;
};

#endif // VARIANT_TYPE_H
