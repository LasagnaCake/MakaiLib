#ifndef CTL_TYPETRAITS_CONVERTER_H
#define CTL_TYPETRAITS_CONVERTER_H

#include "../ctypes.hpp"
#include "containers.hpp"

template<class T> struct UnsignedType;
template<class T> struct SignedType;

template<>	struct UnsignedType<char>:					TypeContainer<unsigned char>		{};
template<>	struct UnsignedType<short>:					TypeContainer<unsigned int>			{};
template<>	struct UnsignedType<int>:					TypeContainer<unsigned short>		{};
template<>	struct UnsignedType<long>:					TypeContainer<unsigned long>		{};
template<>	struct UnsignedType<long long>:				TypeContainer<unsigned long long>	{};

template<>	struct UnsignedType<unsigned char>:			TypeContainer<unsigned char>		{};
template<>	struct UnsignedType<unsigned short>:		TypeContainer<unsigned int>			{};
template<>	struct UnsignedType<unsigned int>:			TypeContainer<unsigned short>		{};
template<>	struct UnsignedType<unsigned long>:			TypeContainer<unsigned long>		{};
template<>	struct UnsignedType<unsigned long long>:	TypeContainer<unsigned long long>	{};

template<>	struct SignedType<char>:					TypeContainer<char>					{};
template<>	struct SignedType<short>:					TypeContainer<int>					{};
template<>	struct SignedType<int>:						TypeContainer<short>				{};
template<>	struct SignedType<long>:					TypeContainer<long>					{};
template<>	struct SignedType<long long>:				TypeContainer<long long>			{};

template<>	struct SignedType<unsigned char>:			TypeContainer<char>					{};
template<>	struct SignedType<unsigned short>:			TypeContainer<int>					{};
template<>	struct SignedType<unsigned int>:			TypeContainer<short>				{};
template<>	struct SignedType<unsigned long>:			TypeContainer<long>					{};
template<>	struct SignedType<unsigned long long>:		TypeContainer<long long>			{};

template<class T> using AsUnsigned	= typename UnsignedType<T>::Type;
template<class T> using AsSigned	= typename SignedType<T>::Type;

template<class T> struct NonReferenceType:		TypeContainer<T> {};
template<class T> struct NonReferenceType<T&>:	TypeContainer<T> {};
template<class T> struct NonReferenceType<T&&>:	TypeContainer<T> {};

template<class T> using AsNonReference = typename NonReferenceType<T>::Type;

template<class T> struct NonConstType:					TypeContainer<T> {};
template<class T> struct NonConstType<T const>:			TypeContainer<T> {};

template<class T> using AsNonConst = typename NonConstType<T>::Type;

template<class T> struct NonVolatileType:				TypeContainer<T> {};
template<class T> struct NonVolatileType<T volatile>:	TypeContainer<T> {};

template<class T> using AsNonVolatile = typename NonVolatileType<T>::Type;

template<class T> struct NonCVType:						TypeContainer<T> {};
template<class T> struct NonCVType<T const>:			TypeContainer<T> {};
template<class T> struct NonCVType<T volatile>:			TypeContainer<T> {};
template<class T> struct NonCVType<T const volatile>:	TypeContainer<T> {};

template<class T> using AsNonCV = typename NonCVType<T>::Type;

template<class T> struct NormalType:	TypeContainer<AsNonCV<AsNonReference<T>>> {};

template<class T> using AsNormal = typename NormalType<T>::Type;

template<class T> struct NonPointerType:		TypeContainer<AsNonCV<T>> {};
template<class T> struct NonPointerType<T*>:	TypeContainer<AsNonCV<T>> {};

template<class T> using AsNonPointer = typename NonPointerType<T>::Type;

template<class T> using AsPointer = AsNonReference<T>*;

template<class T> struct ReferenceType:			TypeContainer<T&> {};
template<class T> struct ReferenceType<T&>:		TypeContainer<T&> {};
template<class T> struct ReferenceType<T&&>:	TypeContainer<T&> {};

template<class T> using AsReference	= typename ReferenceType<T>::Type;

template<class T> struct TemporaryType:			TypeContainer<T&&> {};
template<class T> struct TemporaryType<T&>:		TypeContainer<T&&> {};
template<class T> struct TemporaryType<T&&>:	TypeContainer<T&&> {};

template<class T> using AsTemporary	= typename TemporaryType<T>::Type;

template<class T> struct ConstantType:				TypeContainer<T const>			{};
template<class T> struct VolatileType:				TypeContainer<T volatile>		{};
template<class T> struct ConstVolatileType:			TypeContainer<T const volatile>	{};

template<class T> using AsConstant		= typename ConstantType<T>::Type;
template<class T> using AsVolatile		= typename VolatileType<T>::Type;
template<class T> using AsConstVolatile	= typename ConstVolatileType<T>::Type;

template<class T>			struct ArrayElementType:		TypeContainer<T>	{};
template<class T>			struct ArrayElementType<T[]>:	TypeContainer<T>	{};
template<class T, usize N>	struct ArrayElementType<T[N]>:	TypeContainer<T>	{};

template<class T> using AsArrayElement	= typename ArrayElementType<T>::Type;

template<class T>			struct NonExtentType:		TypeContainer<T>								{};
template<class T>			struct NonExtentType<T[]>:	TypeContainer<typename NonExtentType<T>::Type>	{};
template<class T, usize N>	struct NonExtentType<T[N]>:	TypeContainer<typename NonExtentType<T>::Type>	{};

template<class T> using AsNonExtent	= typename NonExtentType<T>::Type;

template<class T>			struct ArrayType:		TypeContainer<T[]> {};
template<class T>			struct ArrayType<T[]>:	TypeContainer<T[]> {};
template<class T, usize N>	struct ArrayType<T[N]>:	TypeContainer<T[]> {};

template<class T> using AsArray	= typename ArrayType<T>::Type;

template<class T, usize N>	struct FixedArrayType:			TypeContainer<T[N]> {};
template<class T, usize N>	struct FixedArrayType<T[], N>:	TypeContainer<T[N]> {};
template<class T, usize N>	struct FixedArrayType<T[N], N>:	TypeContainer<T[N]> {};

template<class T, usize N> using AsFixedArray = typename FixedArrayType<T, N>::Type;

template<class T>					struct ReturnType:				TypeContainer<T> {};
template<class T, class... Args>	struct ReturnType<T(Args...)>:	TypeContainer<T> {};

template<class T> using AsReturn = typename ReturnType<T>::Type;

template<typename T> AsTemporary<T> declval();

#endif // CTL_TYPETRAITS_CONVERTER_H
