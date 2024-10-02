#ifndef CTL_TYPETRAITS_BASIC_TRAITS_H
#define CTL_TYPETRAITS_BASIC_TRAITS_H

#include <typeinfo>

#include "../ctypes.hpp"
#include "converter.hpp"
#include "metaprogramming.hpp"

namespace Type {
	namespace Impl {
		namespace Partial {
			template<typename T>	struct IsPointer:			FalseType	{};
			template<typename T>	struct IsPointer<T*>:		TrueType	{};

			template<typename T>	struct IsLeftReference:		FalseType	{};
			template<typename T>	struct IsLeftReference<T&>:	TrueType	{};

			template<typename T>	struct IsTemporary:			FalseType	{};
			template<typename T>	struct IsTemporary<T&&>:	TrueType	{};

			// cppreference
			template<typename T>			struct IsMemberPointer:			FalseType	{};
			template<typename T, class C>	struct IsMemberPointer<T C::*>:	TrueType	{};

			// Hooray... it's intrinsics...
			template<typename T>	struct IsUnion:			BooleanConstant<__is_union(T)>	{};
			template<typename T>	struct IsEnumerator:	BooleanConstant<__is_enum(T)>	{};

			// cppreference
			template<class T>
			BooleanConstant<!IsUnion<AsNonCV<T>>::value> isClass(int T::*);
			template<class>
			FalseType isClass(...);

			// cppreference
			template<typename Base>
			TrueType isPointerConvertible(const volatile Base*);
			template<typename>
			FalseType isPointerConvertible(const volatile void*);

			// cppreference
			template<typename T, typename Base>
			auto isDerived(int) -> decltype(isPointerConvertible<Base>(static_cast<T*>(nullptr)));
			template<typename, typename>
			auto isDerived(...) -> TrueType;

			// cppreference
			template<class T>
			auto isReturnable(int) -> decltype(
				void(static_cast<T(*)()>(nullptr)), TrueType {}
			);
			template<class>
			auto isReturnable(...) -> FalseType;

			// cppreference
			template<class From, class To>
			auto isImplicit(int) -> decltype(
				void(declval<void(&)(To)>()(declval<From>())), TrueType {}
			);
			template<class, class>
			auto isImplicit(...) -> FalseType;

			// Based off of: https://stackoverflow.com/a/38181716
			template <class, class T, class... Args>	struct IsConstructible: FalseType {};
			template <class T, class... Args>			struct IsConstructible<
				VoidType<decltype(::new T(declval<Args>()...))
			>, T, Args...> : TrueType {};
		}

		template<typename A, typename B>	struct IsEqual:			FalseType	{};
		template<typename T>				struct IsEqual<T, T>:	TrueType	{};

		template<typename T>	struct IsConstant:			FalseType 	{};
		template<typename T>	struct IsConstant<T const>:	TrueType	{};

		template<typename T>	struct IsVolatile:				FalseType 	{};
		template<typename T>	struct IsVolatile<T volatile>:	TrueType	{};

		template<typename T>	struct IsInteger:	BooleanConstant<
			requires (T t, T* p, void (*f)(T)) {
				reinterpret_cast<T>(t);
				f(0);
				p + t;
			}
		> {};

		template<typename T>	struct IsReal:		TrueType::Or<
			IsEqual<float,			AsNonCV<T>>
		,	IsEqual<double,			AsNonCV<T>>
		,	IsEqual<long double,	AsNonCV<T>>
		> {};

		template<typename T>	struct IsNumber:	TrueType::Or<IsInteger<T>, IsReal<T>> {};

		template<typename T>	struct IsUnion:			Partial::IsUnion<AsNonCV<T>>		{};
		template<typename T>	struct IsEnumerator:	Partial::IsEnumerator<AsNonCV<T>>	{};

		template<typename T>	struct IsVoid:			FalseType	{};
		template<>				struct IsVoid<void>:	TrueType	{};

		template<class T>					struct IsAutoArray:			FalseType	{};
		template<class T>					struct IsAutoArray<T[]>:	TrueType	{};

		template<class T>					struct IsFixedArray:		FalseType	{};
		template<class T, std::size_t N>	struct IsFixedArray<T[N]>:	TrueType	{};

		template<typename T>				struct IsArray:				TrueType::Or<IsAutoArray<T>, IsFixedArray<T>>	{};

		template<typename T>	struct IsNullPointer:			FalseType	{};
		template<>				struct IsNullPointer<nulltype>:	TrueType	{};

		template<typename T>	struct IsPointer:		Partial::IsPointer<AsNonCV<T>>			{};
		template<typename T>	struct IsMemberPointer:	Partial::IsMemberPointer<AsNonCV<T>>	{};
		template<typename T>	struct IsLeftReference:	Partial::IsLeftReference<AsNonCV<T>>	{};
		template<typename T>	struct IsTemporary:		Partial::IsTemporary<AsNonCV<T>>		{};
		template<typename T>	struct IsReference:		TrueType::Or<IsLeftReference<T>, IsTemporary<T>>	{};

		template<class T>						struct IsFunction:				FalseType	{};
		template<typename R, typename... Args>	struct IsFunction<R(Args...)>:	TrueType	{};

		template<typename T> requires (IsNumber<T>::value)	struct IsUnsigned:	BooleanConstant<(T(-1) > T(0))>				{};
		template<typename T> requires (IsNumber<T>::value)	struct IsSigned:	BooleanConstant<(!IsUnsigned<T>::value)>	{};

		// cppreference
		template<typename T>	struct IsClass:			decltype(Partial::isClass<T>(nullptr))	{};

		// cppreference
		template<typename T, typename Base>	struct IsDerived:	BooleanConstant<
			IsClass<Base>::value && IsClass<T>::value
		&&	decltype(Partial::isDerived<T, Base>(0))::value
		>	{};
		template<class T, class To> struct IsConvertible:	BooleanConstant<
			(
				decltype(Partial::isReturnable<To>(0))::value
			&&	decltype(Partial::isImplicit<T, To>(0))::value
			)
		||	TrueType::And<IsVoid<T>, IsVoid<To>>::value
		> {};

		// cppreference
		template<class T, class To>
		struct IsNothrowConvertible: TrueType::And<IsVoid<T>, IsVoid<To>> {};
		template<class T, class To>
			requires requires {
				static_cast<To(*)()>(nullptr);
				{ declval<void(&)(To) noexcept>()(declval<T>()) } noexcept;
			}
		struct IsNothrowConvertible<T, To>: TrueType {};

		template<typename T, typename... Args>	struct IsConstructible:	Partial::IsConstructible<VoidType<>, T, Args...>	{};
	}

	template<typename A, typename B>
	concept Equal = Impl::IsEqual<A, B>::value;

	template<typename A, typename B>
	concept Different = !Equal<A, B>;

	template<typename T, typename To>
	concept Convertible = Impl::IsConvertible<T, To>::value;

	template<typename T, typename To>
	concept NothrowConvertible = Impl::IsNothrowConvertible<T, To>::value;

	template<typename T>
	concept Constant = Impl::IsConstant<T>::value;

	template<typename T>
	concept Volatile = Impl::IsVolatile<T>::value;

	template<typename T>
	concept Integer = Impl::IsInteger<T>::value;

	template<typename T>
	concept Real = Impl::IsReal<T>::value;

	template<typename T>
	concept Number = Integer<T> || Real<T>;

	template<typename T>
	concept AutoArray = Impl::IsArray<T>::value;

	template<typename T>
	concept FixedArray = Impl::IsArray<T>::value;

	template<typename T>
	concept Array = Impl::IsArray<T>::value;

	template<typename T>
	concept Void = Impl::IsVoid<T>::value;

	template <typename T>
	concept NonVoid = !Void<T>;

	template<typename T>
	concept Null = Impl::IsNullPointer<T>::value;

	template<typename T>
	concept NonNull = !Null<T>;

	template<typename T>
	concept Unsigned = Impl::IsUnsigned<T>::value;

	template<typename T>
	concept Signed = Impl::IsSigned<T>::value;

	template<typename T>
	concept SignedInteger = Signed<T> && Integer<T>;

	template<typename T>
	concept UnsignedInteger = Unsigned<T> && Integer<T>;

	template<typename T>
	concept Function = Impl::IsFunction<T>::value;

	template<typename T>
	concept Pointer = Impl::IsPointer<T>::value;

	template<typename T>
	concept MemberPointer = Impl::IsMemberPointer<T>::value;

	template<typename T>
	concept LeftReference = Impl::IsLeftReference<T>::value;

	template<typename T>
	concept Temporary = Impl::IsTemporary<T>::value;

	template<typename T>
	concept Reference = LeftReference<T> || Temporary<T>;

	template <typename T>
	concept Numerable = Convertible<size_t, T>;

	template<typename T>
	concept Enumerator = Impl::IsEnumerator<T>::value;

	template<typename T>
	concept Union = Impl::IsUnion<T>::value;

	template<typename T>
	concept Class = Impl::IsClass<T>::value;

	template <typename T>
	concept NumerableEnum = Enumerator<T> && Numerable<T>;

	template <typename T>
	concept StandardEnum = Enumerator<T> && !Numerable<T>;

	template<typename T, typename Base>
	concept Derived	= Impl::IsDerived<T, Base>::value;

	template<typename T, typename Base>
	concept Subclass = Derived<T, Base> && Different<T, Base>;

	template <typename T>
	concept Primitive = !(Class<T> || Union<T> || StandardEnum<T>);

	template <typename T>
	concept Fundamental = Number<T> || Void<T> || Null<T>;

	template <typename T>
	concept Derivable = !Primitive<T>;

	template<typename T>
	concept Scalar = Integer<T> || Real<T> || Pointer<T> || MemberPointer<T> || Null<T>;

	template <class T, typename... Args>
	concept Constructible = Impl::IsConstructible<T, Args...>::value;

	template <typename A, typename B>
	concept Addable = requires (A a, B b) {a + b;};

	template <typename A, typename B>
	concept Subtractable = requires (A a, B b) {a - b;};

	template <typename A, typename B>
	concept Multipliable = requires (A a, B b) {a * b;};

	template <typename A, typename B>
	concept Dividable = requires (A a, B b) {a / b;};

	template <typename A, typename B>
	concept Modulable = requires (A a, B b) {a % b;};

	template <typename A, typename B>
	concept AddAssignable = requires (A a, B b) {a += b;};

	template <typename A, typename B>
	concept SubAssignable = requires (A a, B b) {a -= b;};

	template <typename A, typename B>
	concept MulAssignable = requires (A a, B b) {a *= b;};

	template <typename A, typename B>
	concept DivAssignable = requires (A a, B b) {a /= b;};

	template <typename A, typename B>
	concept ModAssignable = requires (A a, B b) {a %= b;};

	template <typename T>
	concept PostIncrementable = requires (T t) {t++;};

	template <typename T>
	concept PostDecrementable = requires (T t) {t--;};

	template <typename T>
	concept PreIncrementable = requires (T t) {++t;};

	template <typename T>
	concept PreDecrementable = requires (T t) {--t;};

	namespace Bitwise {
		template <typename A, typename B>
		concept Andable = requires (A a, B b) {a & b;};

		template <typename A, typename B>
		concept Orable = requires (A a, B b) {a | b;};

		template <typename A, typename B>
		concept Xorable = requires (A a, B b) {a ^ b;};

		template <typename A, typename B>
		concept Expressionable =
			Andable<A, B>
		&&	Orable<A, B>
		&&	Xorable<A, B>
		;

		template <typename A, typename B>
		concept AndAssignable = requires (A a, B b) {a &= b;};

		template <typename A, typename B>
		concept OrAssignable = requires (A a, B b) {a |= b;};

		template <typename A, typename B>
		concept XorAssignable = requires (A a, B b) {a ^= b;};

		template <typename A, typename B>
		concept Assignable =
			AndAssignable<A, B>
		&&	OrAssignable<A, B>
		&&	XorAssignable<A, B>
		;

		template <typename T>
		concept Negatable = requires (T t) {~t;};

		template<typename A, typename B>
		concept All =
			Expressionable<A, B>
		&&	Assignable<A, B>
		&&	Negatable<A>
		&&	Negatable<B>
		;
	}

	namespace Logical {
		template <typename A, typename B>
		concept Andable = requires (A a, B b) {a && b;};

		template <typename A, typename B>
		concept Orable = requires (A a, B b) {a || b;};

		template <typename T>
		concept Negatable = requires (T t) {!t;};

		template <typename A, typename B>
		concept All =
			Andable<A, B>
		&&	Orable<A, B>
		&&	Negatable<A>
		&&	Negatable<B>
		;
	}

	namespace Comparable {
		template <typename A, typename B>
		concept Equals = requires (A a, B b) {a == b;};

		template <typename A, typename B>
		concept NotEquals = requires (A a, B b) {a != b;};

		template <typename A, typename B>
		concept Lesser = requires (A a, B b) {a < b;};

		template <typename A, typename B>
		concept Greater = requires (A a, B b) {a > b;};

		template <typename A, typename B>
		concept LesserEquals = requires (A a, B b) {a <= b;};

		template <typename A, typename B>
		concept GreaterEquals = requires (A a, B b) {a >= b;};

		template <typename A, typename B>
		concept Threeway = requires (A a, B b) {a <=> b;};

		template <typename A, typename B>
		concept All =
			Equals<A, B>
		&&	NotEquals<A, B>
		&&	Lesser<A, B>
		&&	Greater<A, B>
		&&	LesserEquals<A, B>
		&&	GreaterEquals<A, B>
		;
	}

	namespace Stream {
		template <typename A, typename B>
		concept Insertible = requires (A a, B b) {a << b;};

		template <typename A, typename B>
		concept Extractible = requires (A a, B b) {a >> b;};

		template <typename A, typename B>
		concept Expressionable = Insertible<A, B> && Extractible<A, B>;

		template <typename A, typename B>
		concept InsAssignable = requires (A a, B b) {a <<= b;};

		template <typename A, typename B>
		concept ExtAssignable = requires (A a, B b) {a >>= b;};

		template <typename A, typename B>
		concept Assignable = InsAssignable<A, B> && ExtAssignable<A, B>;

		template <typename A, typename B>
		concept All = Expressionable<A, B> && Assignable<A, B>;
	}

	template <typename A, typename B>
	concept Calculable =
			Addable<A, B>
		&&	Subtractable<A, B>
		&&	Multipliable<A, B>
		&&	Dividable<A, B>
	;

	template <typename A, typename B>
	concept CalcAssignable =
			AddAssignable<A, B>
		&&	SubAssignable<A, B>
		&&	MulAssignable<A, B>
		&&	DivAssignable<A, B>
	;

	template <typename A, typename B>
	concept Arithmetic = Calculable<A, B> && CalcAssignable<A, B>;

	template <typename A, typename B>
	concept HasModulo = Modulable<A, B> && ModAssignable<A, B>;

	/**
	* A 'Safe' type must not be:
	*	1)	A pointer (null or non-null),
	*	2)	A reference,
	*	3)	A function, or
	*	4)	Void
	* Ergo, a 'raw value' type.
	*/
	template <typename T>
	concept Safe = !(
			Pointer<T>
		||	Reference<T>
		||  Function<T>
		||	Void<T>
		||	Null<T>
	);

	template <typename T>
	concept SafeArray = Array<T> && Safe<T>;

	/**
	* A 'Mutable' is a 'Safe' type that isn't an array or constant.
	*/
	template <typename T>
	concept Mutable = Safe<T> && !(Array<T> || Constant<T>);

	namespace {
		template<class Lambda, int=(Lambda{}(), 0)>
		constexpr bool isConstexpr(Lambda)	{return true;	}
		constexpr bool isConstexpr(...)		{return false;	}

		template<typename T, typename = void>
		constexpr bool isDefined = false;

		template<typename T>
		constexpr bool isDefined<T, decltype(typeid(T), void())> = true;
	}

	template <typename T>
	concept CompileTimeable = isConstexpr([]{return T();});

	template <class T>
	concept Defined = isDefined<T>;
}

#endif // CTL_TYPETRAITS_TRAITS_H
