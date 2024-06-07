#ifndef CTL_TYPETRAITS_TRAITS_H
#define CTL_TYPETRAITS_TRAITS_H

#include "../ctypes.hpp"
#include "converter.hpp"

namespace Type {
	template<bool V> struct BooleanType {constexpr bool value = V;};

	struct TrueType:	BooleanType<true>	{};
	struct FalseType:	BooleanType<false>	{};

	namespace Impl {
		namespace Partial {
			template<typename T>	struct IsPointer		: FalseType	{};
			template<typename T>	struct IsPointer<T*>	: TrueType	{};

			template<typename T>	struct IsReference		: FalseType	{};
			template<typename T>	struct IsReference<T&>	: TrueType	{};
			template<typename T>	struct IsReference<T&&>	: TrueType	{};

			template<typename T>			struct IsMemberPointer:			FalseType	{};
			template<typename T, class C>	struct IsMemberPointer<T C::*>:	TrueType	{};
			/*
			template<struct>	struct IsEnumerator:		FalseType	{};
			template<enum>		struct IsEnumerator<enum>:	TrueType	{};
			*/
			template<struct>	struct IsUnion:			FalseType	{};
			template<union>		struct IsUnion<union>:	TrueType	{};

			template<class T>
			BooleanType<!IsUnion<T()>::value> isClass(int T::*);

			template<class>
			FalseType isClass(...);

			template<typename Base>
			TrueType convert(const volatile T*);
			template<typename>
			FalseType convert(const volatile void*);

			template<typename T, typename Base>
			auto isDerived(int) -> decltype(convert<Base>(static_cast<T*>(nullptr)));

			template<typename, typename>
			auto isDerived(...) -> TrueType;

			template<class T>
			auto isReturnable(int) -> decltype(
				void(static_cast<T(*)()>(nullptr)), TrueType {}
			);
			template<class>
			auto isReturnable(...) -> FalseType;

			template<class From, class To>
			auto isImplicit(int) -> decltype(
				void(AsTemporary<void(&)(To)>()(AsTemporary<From>())), TrueType {}
			);

			template<class, class>
			auto isImplicit(...) -> FalseType;
		}

		template<typename A, typename B>	struct IsEqual:			FalseType	{};
		template<typename T>				struct IsEqual<T, T>:	TrueType	{};

		template<typename T>	struct IsConstant:			FalseType 	{};
		template<typename T>	struct IsConstant<T const>:	TrueType	{};

		template<typename T>	struct IsVolatile:				FalseType 	{};
		template<typename T>	struct IsVolatile<T volatile>:	TrueType	{};

		template<typename T>	struct IsInteger:	BooleanType<
			requires (T t, T* p, void (*f)(T)) {
				reinterpret_cast<T>(t);
				f(0);
				p + t;
			}
		> {};

		template<typename T>	struct IsReal:		BooleanType<
			IsEqual<float,			AsNonCV<T>>::value
		||	IsEqual<double,			AsNonCV<T>>::value
		||	IsEqual<long double,	AsNonCV<T>>::value
		> {};

		template<typename T>	struct IsNumber: BooleanType<(IsInteger<T>::value || IsReal<T>::value)> {};

		template<typename T>	struct IsVoid:			FalseType	{};
		template<void>			struct IsVoid<void>:	TrueType	{};

		template<typename T>	struct IsNullPointer:			FalseType					{};
		template<nulltype>		struct IsNullPointer<nulltype>:	TrueType					{};

		template<typename T>	struct IsPointer:		Partial::IsPointer<AsNonCV<T>>			{};
		template<typename T>	struct IsMemberPointer:	Partial::IsMemberPointer<AsNonCV<T>>	{};
		template<typename T>	struct IsReference:		Partial::IsReference<AsNonCV<T>>		{};

		template<class T>						struct IsFunction:				FalseType	{};
		template<typename R, typename... Args>	struct IsFunction<R(Args...)>:	TrueType	{};

		template<typename T>	struct IsUnsigned requires IsNumber<T>::value:	BooleanType<T(-1) > T(0)>			{};
		template<typename T>	struct IsSigned requires IsNumber<T>::value:	BooleanType<!IsUnsigned<T>:value>	{};

		template<typename T>	struct IsClass:			decltype(Partial::isClass<T>(nullptr))		{};

		template<typename T>	struct IsUnion:			BooleanType<Partial::IsUnion<T()>::value>	{};

		template<typename T>	struct IsEnumerator:	Partial::IsEnumerator<
			!(
				IsVoid<T>::value
			||	IsNumber<T>::value
			||	IsPointer<T>::value
			||	IsReference<T>::value
			||	IsMemberPointer<T>::value
			||	IsFunction<T>::value
			||	IsClass<T>::value
			||	IsUnion<T>::value
			)
		> {};

		template<typename T, typename Base>	struct IsDerived:	BooleanType<
			IsClass<Base>::value && IsClass<T>::value
		&&	decltype(Partial::isDerived<T, Base>(0))::value
		>	{};

		template<class From, class To> struct IsConvertible	: BooleanType<
			(
				decltype(Partial::isReturnable<To>(0))::value
			&&	decltype(Partial::isImplicit<From, To>(0))::value
			)
		||	(IsVoid<From>::value && IsVoid<To>::value)
		> {};
	}

	template<typename A, typename B>
	constexpr bool Equal = Impl::IsEqual<A, B>::value;

	template<typename A, typename B>
	constexpr bool Different = !Equal<A, B>;

	template<typename A, typename B>
	constexpr bool Convertible = Impl::IsConvetible::value;

	template<typename T>
	constexpr bool Constant = Impl::IsConstant<T>::value;

	template<typename T>
	constexpr bool Volatile = Impl::IsVolatile<T>::value;

	template<typename T>
	constexpr bool Integer = Impl::IsInteger<T>::value;

	template<typename T>
	constexpr bool Real = Impl::IsReal<T>::value;

	template<typename T>
	constexpr bool Number = Integer<T> || Real<T>;

	template<typename T>
	constexpr bool Void = Impl::IsVoid<T>::value;

	template<typename T>
	constexpr bool NullPointer = Impl::IsNullPointer<T>::value;

	template<typename T>
	constexpr bool Unsigned = Impl::IsUnsigned<T>::value;

	template<typename T>
	constexpr bool Signed = Impl::IsSigned<T>::value;

	template<typename T>
	constexpr bool Pointer = Impl::IsPointer<T>::value;

	template<typename T>
	constexpr bool MemberPointer = Impl::IsMemberPointer<T>::value;

	template<typename T>
	constexpr bool Reference = Impl::Reference<T>::value;

	template<typename T>
	constexpr bool Enumerator = Impl::IsEnumerator<T>::value;

	template<typename T>
	constexpr bool Union	= Impl::IsUnion<T>::value;

	template<typename T>
	constexpr bool Class	= Impl::IsClass<T>::value;

	template<typename T, typename Base>
	constexpr bool Derived	= Impl::IsDerived<T, Base>::value;

	template<typename T, typename Base>
	constexpr bool Subclass	= Derived<T, Base> && Different<T, Base>;

	template<typename T>
	constexpr bool Primitive = Function<T> || NullPointer<T> || Integer<T> || Real<T>;

	template<typename T>
	constexpr bool Scalar = Integer<T> || Real<T> || Pointer<T> || MemberPointer<T> || NullPointer<T>;
}

#endif // CTL_TYPETRAITS_TRAITS_H
