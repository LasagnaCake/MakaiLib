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

template<class T> using AsNonReference	= typename NonReferenceType<T>::Type;

template<class T> struct NonPointerType:		TypeContainer<T> {};
template<class T> struct NonPointerType<T*>:	TypeContainer<T> {};

template<class T> using AsNonPointer	= typename NonPointerType<T>::Type;

template<class T> struct NonConstType:					TypeContainer<T> {};
template<class T> struct NonConstType<const T>:			TypeContainer<T> {};

template<class T> using AsNonConst		= typename NonConstType<T>::Type;

template<class T> struct NonVolatileType:				TypeContainer<T> {};
template<class T> struct NonVolatileType<volatile T>:	TypeContainer<T> {};

template<class T> using AsNonVolatile	= typename NonVolatileType<T>::Type;

template<class T> struct NonCVType	{typedef AsNonConst<AsNonVolatile<T>> Type;};

template<class T> using AsNonCV		= typename NonCVType<T>::Type;

template<class T> struct NormalType	{typedef AsNonConst<AsNonCV<T>> Type;};

template<class T> using AsNormal	= typename NormalType<T>::Type;

template<class T> struct ReferenceType:			TypeContainer<T&> {};
template<class T> struct ReferenceType<T&>:		TypeContainer<T&> {};
template<class T> struct ReferenceType<T&&>:	TypeContainer<T&> {};

template<class T> using AsReference	= typename ReferenceType<T>::Type;

template<class T> struct TemporaryType:			TypeContainer<T&&> {};
template<class T> struct TemporaryType<T&>:		TypeContainer<T&&> {};
template<class T> struct TemporaryType<T&&>:	TypeContainer<T&&> {};

template<class T> using AsTemporary	= typename TemporaryType<T>::Type;

template<class T> struct ConstantType:				TypeContainer<T const> {};
template<class T> struct ConstantType<T const>:		TypeContainer<T const> {};

template<class T> using AsConstant		= typename ConstantType<T>::Type;

template<class T> struct VolatileType:				TypeContainer<T volatile> {};
template<class T> struct VolatileType<T volatile>:	TypeContainer<T volatile> {};

template<class T> using AsVolatile		= typename VolatileType<T>::Type;

template<class T>			struct NonArrayType:		TypeContainer<T>	{};
template<class T>			struct NonArrayType<T[]>:	TypeContainer<T>	{};
template<class T, usize N>	struct NonArrayType<T[N]>:	TypeContainer<T>	{};

template<class T> using AsNonArray	= typename NonArrayType<T>::Type;
/*
template<class T>			struct NonArrayRecursiveType:		TypeContainer<T>										{};
template<class T>			struct NonArrayRecursiveType<T[]>:	TypeContainer<typename NonArrayRecursiveType<T>::Type>	{};
template<class T, usize N>	struct NonArrayRecursiveType<T[N]>:	TypeContainer<typename NonArrayRecursiveType<T>::Type>	{};

template<class T> using AsNonArrayRecursive	= typename AsNonArrayRecursive<T>::Type;
*/

template<class T>			struct ArrayType:		TypeContainer<T[]> {};
template<class T>			struct ArrayType<T[]>:	TypeContainer<T[]> {};
template<class T, usize N>	struct ArrayType<T[N]>:	TypeContainer<T[]> {};

template<class T> using AsArray	= typename ArrayType<T>::Type;

template<class T, usize N>	struct FixedArrayType:			TypeContainer<T[N]> {};
template<class T, usize N>	struct FixedArrayType<T[], N>:	TypeContainer<T[N]> {};
template<class T, usize N>	struct FixedArrayType<T[N], N>:	TypeContainer<T[N]> {};

template<class T, usize N> using AsFixedArray	= typename FixedArrayType<T, N>::Type;

template<class T>					struct ReturnType:				TypeContainer<T> {};
template<class T, class... Args>	struct ReturnType<T(Args...)>:	TypeContainer<T> {};

template<class T> using AsReturn	= typename ReturnType<T>::Type;

template<typename T> AsTemporary<T> declval();

#endif // CTL_TYPETRAITS_CONVERTER_H
