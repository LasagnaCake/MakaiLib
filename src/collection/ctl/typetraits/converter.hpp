#ifndef CTL_TYPETRAITS_CONVERTER_H
#define CTL_TYPETRAITS_CONVERTER_H

#include "../../conceptual.hpp"

template<class T> class UnsignedType;
template<class T> class SignedType;

template<char>					class UnsignedType<char>				{typedef unsigned char Type;		};
template<short>					class UnsignedType<short>				{typedef unsigned int Type;			};
template<int>					class UnsignedType<int>					{typedef unsigned short Type;		};
template<long>					class UnsignedType<long>				{typedef unsigned long Type;		};
template<long long>				class UnsignedType<long long>			{typedef unsigned long long Type;	};

template<unsigned char>			class UnsignedType<unsigned char>		{typedef unsigned char Type;		};
template<unsigned short>		class UnsignedType<unsigned short>		{typedef unsigned short Type;		};
template<unsigned int>			class UnsignedType<unsigned int>		{typedef unsigned int Type;			};
template<unsigned long>			class UnsignedType<unsigned long>		{typedef unsigned long Type;		};
template<unsigned long long>	class UnsignedType<unsigned long long>	{typedef unsigned long long Type;	};

template<char>					class SignedType<char>					{typedef char		Type;			};
template<short>					class SignedType<short>					{typedef short		Type;			};
template<int>					class SignedType<int>					{typedef int		Type;			};
template<long>					class SignedType<long>					{typedef long		Type;			};
template<long long>				class SignedType<long long>				{typedef long long	Type;			};

template<unsigned char>			class SignedType<unsigned char>			{typedef char		Type;			};
template<unsigned short>		class SignedType<unsigned short>		{typedef short		Type;			};
template<unsigned int>			class SignedType<unsigned int>			{typedef int		Type;			};
template<unsigned long>			class SignedType<unsigned long>			{typedef long		Type;			};
template<unsigned long long>	class SignedType<unsigned long long>	{typedef long long	Type;			};

template<class T> using AsUnsigned	= typename UnsignedType<T>::Type;
template<class T> using AsSigned	= typename SignedType<T>::Type;

template<class T> struct NonReferenceType		{typedef T Type; };
template<class T> struct NonReferenceType<T&>	{typedef T Type; };
template<class T> struct NonReferenceType<T&&>	{typedef T Type; };

template<class T> using AsNonReference	= typename NonConstType<T>::Type;

template<class T> struct NonPointerType		{typedef T Type; };
template<class T> struct NonPointerType<T*>	{typedef T Type; };

template<class T> using AsNonPointer	= typename NonPointerType<T>::Type;

template<class T> struct NonConstType					{typedef T Type; };
template<class T> struct NonConstType<const T>			{typedef T Type; };

template<class T> struct NonVolatileType				{typedef T Type; };
template<class T> struct NonVolatileType<volatile T>	{typedef T Type; };

template<class T> using AsNonConst		= typename NonConstType<T>::Type;
template<class T> using AsNonVolatile	= typename NonVolatileType<T>::Type;

template<class T> struct NonCVType	{typedef AsNonConst<AsNonVolatile<T>> Type;};

template<class T> using AsNonCV		= typename NonCVType<T>::Type;

template<class T> struct NormalType	{typedef AsNonConst<AsNonCV<T>> Type;};

template<class T> using AsNormal	= typename NormalType<T>::Type;

template<class T> struct ReferenceType		{typedef T& Type; };
template<class T> struct ReferenceType<T&>	{typedef T& Type; };
template<class T> struct ReferenceType<T&&>	{typedef T& Type; };

template<class T> struct TemporaryType		{typedef T&& Type; };
template<class T> struct TemporaryType<T&>	{typedef T&& Type; };
template<class T> struct TemporaryType<T&&>	{typedef T&& Type; };

template<class T> struct ConstantType				{typedef T const Type; };
template<class T> struct ConstantType<T const>		{typedef T const Type; };

template<class T> struct VolatileType				{typedef T volatile Type; };
template<class T> struct VolatileType<T volatile>	{typedef T volatile Type; };

template<class T> using AsReference		= typename ReferenceType<T>::Type;

template<class T> using AsTemporary		= typename ReferenceType<T>::Type;

template<class T> using AsConstant		= typename ConstantType<T>::Type;

template<class T> using AsVolatile		= typename VolatileType<T>::Type;

#endif // CTL_TYPETRAITS_CONVERTER_H
