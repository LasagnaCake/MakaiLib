#ifndef CTL_CLASS_TEMPLATES_H
#define CTL_CLASS_TEMPLATES_H

#include "ctypes.hpp"
#include "typeinfo.hpp"
#include "namespace.hpp"
#include "typetraits/traits.hpp"

CTL_NAMESPACE_BEGIN

template<class TSelf>
struct SelfIdentified {
	typedef TSelf SelfType;
	typedef TypeInfo<TSelf> Self;
};

template<class TData>
struct Typed {
	typedef TData				DataType;
	typedef DataType const		ConstantType;
	typedef DataType&			ReferenceType;
	typedef DataType&&			TemporaryType;
	typedef ConstantType&		ConstReferenceType;
	typedef DataType*			PointerType;
	typedef const DataType*		ConstPointerType;
};

template<class TKey>
struct Keyed {
	typedef Typed<TKey>	Key;
	typedef TKey		KeyType;
};

template<class TValue>
struct Valued {
	typedef Typed<TValue>	Value;
	typedef TValue			ValueType;
};

template<Type::Integer TIndex>
struct Indexed {
	typedef AsUnsigned<TIndex>	SizeType;
	typedef AsSigned<SizeType>	IndexType;

	constexpr static SizeType MAX_SIZE = TypeInfo<SizeType>::highest;
};

template <usize N, typename... T>
struct NthInPack;

template <typename T, typename... Types>
struct NthInPack<0, T, Types...> {
    typedef T Type;
};
template <usize N, typename T, typename... Types>
struct NthInPack<N, T, Types...> {
    typedef typename NthInPack<N-1, Types...>::Type Type;
};

template<usize N, typename... Types>
using NthType = typename NthInPack<N, Types...>::Type;

template<typename... Types>
using FirstType = typename NthInPack<0, Types...>::Type;

template<typename... Types>
using LastType = typename NthInPack<sizeof...(Types)-1, Types...>::Type;

template<class... T>
struct PackInfo: Decay::Pack<T...> {
	template<usize N> using	Types = NthType<N, T...>;
	typedef FirstType<T...>	FirstType;
	typedef LastType<T...>	LastType;

	constexpr static usize COUNT = sizeof...(T);
};

template<class... BaseTypes>
struct Derived {
	typedef PackInfo<BaseTypes...> Bases;
};

template<class... Types>
struct Polyglot {
	typedef PackInfo<Types...> DataTypes;
};

struct Nulled {
	typedef nulltype NullType;
};

template<class T, auto V>
struct Defaultable {
	constexpr static T defaultValue() {return V;}
};

template<class TReturn>
struct Returnable {
	typedef Typed<TReturn>	Return;
	typedef TReturn			ReturnType;
};

template<class... Types>
struct Argumented {
	typedef PackInfo<Types...>	ArgumentTypes;
};

template<class TFunction>
struct Functional {};

template<class TReturn, class... TArguments>
struct Functional<TReturn(TArguments...)> {
	typedef Decay::AsType<TReturn(TArguments...)> FunctionType;
};

template<class... Types>
struct Throwable {
	typedef PackInfo<Types...>  Errors;
};

template<typename T>
struct StringLiterable {
	typedef StringLiteralType T const*;
};

CTL_NAMESPACE_END

#endif // CTL_CLASS_TEMPLATES_H
