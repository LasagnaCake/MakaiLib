#ifndef CTL_CLASS_TEMPLATES_H
#define CTL_CLASS_TEMPLATES_H

#include "ctypes.hpp"
#include "typeinfo.hpp"
#include "namespace.hpp"
#include "typetraits/traits.hpp"

CTL_NAMESPACE_BEGIN

template<class TSelf>
struct SelfIdentified {
	using SelfType	= TSelf;
	using Self		= TypeInfo<TSelf>;
};

template<class TData>
struct Typed {
	using DataType				= TData;
	using ConstantType			= DataType const;
	using ReferenceType			= DataType&;
	using TemporaryType			= DataType&&;
	using ConstReferenceType	= ConstantType&;
	using PointerType			= DataType*;
	using ConstPointerType		= DataType const*;
};

template<class TKey>
struct Keyed {
	using Key		= Typed<TKey>;
	using KeyType	= TKey;
};

template<class TValue>
struct Valued {
	using Value		= Typed<TValue>;
	using ValueType	= TValue;
};

template<Type::Integer TIndex>
struct Indexed {
	using SizeType	= AsUnsigned<TIndex>;
	using IndexType	= AsSigned<SizeType>;

	constexpr static SizeType MAX_SIZE = TypeInfo<SizeType>::HIGHEST;

	static_assert(IndexType(-1) == -1);
};

template <usize N, typename... T>
struct NthInPack;

template <typename T, typename... Types>
struct NthInPack<0, T, Types...> {
    using Type = T;
};
template <usize N, typename T, typename... Types>
struct NthInPack<N, T, Types...> {
    using Type = typename NthInPack<N-1, Types...>::Type;
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
	using FirstType	= FirstType<T...>;
	using LastType	= LastType<T...>;

	constexpr static usize COUNT = sizeof...(T);
};
template<class... BaseTypes>
struct Derived;

template<class TBase>
struct Derived<TBase> {
	using BaseType = TBase;
};

template<class FirstBase, class... BaseTypes>
struct Derived<FirstBase, BaseTypes...> {
	using Bases = PackInfo<BaseTypes...>;
};

template<class... Types>
struct Polyglot {
	using DataTypes = PackInfo<Types...>;
};

struct Nulled {
	using NullType = nulltype;
};

template<class T>
struct Defaultable {
	constexpr static T defaultValue() {return T();}
};

template<class TReturn>
struct Returnable {
	using Return		= Typed<TReturn>;
	using ReturnType	= TReturn;
};

template<class... Types>
struct Argumented {
	using ArgumentTypes = PackInfo<Types...>;
};

template<class TFunction>
struct Functional {};

template<class TReturn, class... TArguments>
struct Functional<TReturn(TArguments...)> {
	using FunctionType = Decay::AsFunction<TReturn(TArguments...)>;
};

template<class... Types>
struct Throwable {
	using Exceptions = PackInfo<Types...>;
};

template<typename T>
struct StringLiterable {
	using StringLiteralType = typename Typed<T>::ConstPointerType;
};

CTL_NAMESPACE_END

#endif // CTL_CLASS_TEMPLATES_H
