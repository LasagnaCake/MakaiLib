#ifndef CTL_CLASS_TEMPLATES_H
#define CTL_CLASS_TEMPLATES_H

#include <initializer_list>
#include <numeric_limits>

template<class Self>
struct Reflective {
	typedef Self SelfType;
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

template<Type::Integer I>
struct Indexed {
	typedef std::make_unsigned<I>		SizeType;
	typedef std::make_signed<SizeType>	IndexType;

	constexpr static SizeType MAX_SIZE = TypeInfo<SizeType>::HIGHEST;
};

template <uintmax N, typename... T>
struct PackType;

template <typename T, typename... Types>
struct PackType<0, T, Types...> {
    typedef T Type;
};
template <uintmax N, typename T, typename... Types>
struct PackType<N, T, Types...> {
    typedef typename PackType<N-1, Types...>::Type Type;
};

template<uintmax N, typename... Types>
using NthType = typename PackType<N, Types...>::Type;

template<class... Bases>
struct PackInfo {
	template<uintmax N> using	Types = NthType<N, Bases...>;
	typedef Types<0>			FirstType;

	constexpr static uintmax COUNT = sizeof...(bases);
};

template<class... Bases>
struct Derived {
	typedef PackInfo<Bases...> Bases;
};

#endif // CTL_CLASS_TEMPLATES_H
