#ifndef CTL_CLASS_TEMPLATES_H
#define CTL_CLASS_TEMPLATES_H

#include <initializer_list>
#include <numeric_limits>

template<class T>
struct SelfIdentified {
	typedef T SelfType;
	typedef TypeInfo<T> Self;
};

template<class T>
struct Typed {
	typedef T								DataType;
	typedef DataType const					ConstantType;
	typedef DataType&						ReferenceType;
	typedef ConstantType&					ConstReferenceType;
	typedef std::initializer_list<DataType>	ArgumentListType;
	typedef DataType*						PointerType;
	typedef const DataType*					ConstPointerType;
};

template<class K>
struct Keyed {
	typedef Typed<K>	Key;
	typedef K			KeyType;
};

template<class V>
struct Valued {
	typedef Typed<V>	Value;
	typedef V			ValueType;
};

template<Type::Integer I>
struct Indexed {
	typedef std::make_unsigned<I>		SizeType;
	typedef std::make_signed<SizeType>	IndexType;

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

	constexpr static usize COUNT = sizeof...(bases);
};

template<class... BaseTypes>
struct Derived {
	typedef PackInfo<BaseTypes...> Bases;
};

template<class... Types>
struct Polyglot {
	typedef PackInfo<Types...> DataTypes;
};

#endif // CTL_CLASS_TEMPLATES_H
