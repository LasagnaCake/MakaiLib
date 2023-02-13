#ifndef EXTENDED_TYPES_H
#define EXTENDED_TYPES_H

#include <cstdint>

#if		(INTPTR_MAX == INT16_MAX || INTPTR_MAX == INT8_MAX)
#define int long
#endif

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int	   	uint32;
typedef unsigned long long	uint64;
#if		INTPTR_MAX == INT64_MAX
typedef	uint64				uintmax;
#elif	INTPTR_MAX == INT32_MAX
typedef	uint32				uintmax;
#elif	INTPTR_MAX == INT16_MAX
typedef	uint16				uintmax;
#elif	INTPTR_MAX == INT8_MAX
typedef	uint8				uintmax;
#endif

typedef signed char			int8;
typedef signed short		int16;
typedef signed int			int32;
typedef signed long long	int64;
#if		INTPTR_MAX == INT64_MAX
typedef	int64				intmax;
#elif	INTPTR_MAX == INT32_MAX
typedef	int32				intmax;
#elif	INTPTR_MAX == INT16_MAX
typedef	int16				intmax;
#elif	INTPTR_MAX == INT8_MAX
typedef	int8				intmax;
#endif

typedef float				float32;
typedef double				float64;
typedef long double			float128;
#if		INTPTR_MAX == INT64_MAX
typedef	float64				floatmax;
#else
typedef	float32				floatmax;
#endif

typedef uint8	uchar;
typedef uint16	uint;
typedef uint32	ulong;
typedef uint64	ullong;

typedef int64	llong;

#endif // EXTENDED_TYPES_H
