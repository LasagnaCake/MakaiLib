#ifndef CTL_CLASS_TEMPLATES_H
#define CTL_CLASS_TEMPLATES_H

#include <initializer_list>
#include <numeric_limits>
#include "../conceptual.hpp"
#include "ctypes.hpp"
#include "typeinfo.hpp"
#include "typetraits/converter.hpp"

template<class TSelf>
struct SelfIdentified {
	typedef TSelf SelfType;
	typedef TypeInfo<TSelf> Self;
};

template<class TData>
struct Typed {
	typedef TData							DataType;
	typedef DataType const					ConstantType;
	typedef DataType&						ReferenceType;
	typedef DataType&&						TempReferenceType;
	typedef ConstantType&					ConstReferenceType;
	typedef std::initializer_list<DataType>	ArgumentListType;
	typedef DataType*						PointerType;
	typedef const DataType*					ConstPointerType;
};

template<class TError>
struct Exceptionable {
	typedef TError ErrorType;
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
	typedef Unsigned<TIndex>	SizeType;
	typedef Signed<SizeType>	IndexType;

	constexpr static SizeType MAX_SIZE = TypeInfo<SizeType>::HIGHEST;
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

template<class... Bases>
struct PackInfo {
	template<usize N> using	Types = NthType<N, Bases...>;
	typedef FirstType<Bases...>	FirstType;
	typedef LastType<Bases...>	LastType;

	constexpr static usize COUNT = sizeof...(Bases);
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
	typedef decltype(nullptr) NullType;
};

template<class T, auto V>
struct Defaultable {
	constexpr T defaultValue() {return V;}
};

#endif // CTL_CLASS_TEMPLATES_H
