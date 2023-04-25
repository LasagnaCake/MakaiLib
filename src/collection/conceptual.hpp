#ifndef CONCEPTUAL_TYPES_H
#define CONCEPTUAL_TYPES_H

#include <type_traits>
#include <concepts>

namespace Type {
	template <typename T>
	concept Integer = std::integral<T>;

	template <typename T>
	concept Real = std::floating_point<T>;

	template <typename T>
	concept Number = Integer<T> || Real<T>;

	template <typename T>
	concept Unsigned = std::unsigned_integral<T>;

	template <typename T>
	concept Signed = std::signed_integral<T>;

	template <typename T>
	concept Fundamental = std::is_fundamental<T>::value;

	template <typename T>
	concept Compound = std::is_compound<T>::value;

	template <typename T>
	concept Scalar = std::is_scalar<T>::value;

	template <typename T>
	concept Class = std::is_class<T>::value;

	template <typename T>
	concept Union = std::is_union<T>::value;

	template <typename T>
	concept Enumerator = std::is_enum<T>::value;

	template <typename T>
	concept Pointer = std::is_pointer<T>::value;

	template <typename T>
	concept Function = std::is_function<T>::value;

	template <typename T>
	concept Constant = std::is_const<T>::value;

	template <typename T>
	concept Reference = std::is_reference<T>::value;

	template <typename T>
	concept Array = std::is_array<T>::value;

	template <typename T>
	concept Void = std::is_void<T>::value;

	template <typename T>
	concept Null = std::is_null_pointer<T>::value;

	template <typename T>
	concept NonNull = !Null<T>;

	template <typename A, typename B>
	concept Equal = std::same_as<A, B>;

	template <typename A, typename B>
	concept Different = !std::same_as<A, B>;

	template <typename FROM, typename TO>
	concept Convertible = std::convertible_to<FROM, TO>;

	template <class T>
	concept Abstract = std::is_abstract<T>::value;

	template <class T, typename... ARGS>
	concept Constructible = std::constructible_from<T, ARGS...>;

	template <typename T>
	concept Numerable = Convertible<size_t, T>;

	template <typename T>
	concept NumerableEnum = Enumerator<T> && Numerable<T>;

	template <typename T>
	concept StandardEnum = Enumerator<T> && !Numerable<T>;

	template <typename T>
	concept Primitive = !(Class<T> || Union<T> || StandardEnum<T>);

	template <typename T>
	concept Derivable = !Primitive<T>;

	template <class T, class DERIVED>
	concept Base = std::derived_from<DERIVED, T>;

	template <class T, class BASE>
	concept Derived = std::derived_from<T, BASE>;

	template <class T, class BASE>
	concept Subclass = Derived<T, BASE>  && Different<T, BASE>;

	template <class A, class B>
	concept Related = std::common_with<A, B>;

	template <class A, class B>
	concept Unrelated = !std::common_with<A, B>;

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
	concept Comparable = requires (A a, B b) {
		a == b;
		a != b;
		a >= b;
		a <= b;
		a > b;
		a < b;
	};

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
}

#endif // CONCEPTUAL_TYPES_H
