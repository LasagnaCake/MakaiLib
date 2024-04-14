#ifndef CTL_EXTENDED_TYPES_H
#define CTL_EXTENDED_TYPES_H

#if		INTPTR_MAX == INT64_MAX
#define CPU_ARCH 64
#elif	INTPTR_MAX == INT32_MAX
#define CPU_ARCH 32
#elif	INTPTR_MAX == INT16_MAX
#define CPU_ARCH 16
#else
#define CPU_ARCH 8
#endif

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned long	   	uint32;
typedef unsigned long long	uint64;
#if		CPU_ARCH == 64
typedef	uint64				uintmax;
#elif	CPU_ARCH == 32
typedef	uint32				uintmax;
#elif	CPU_ARCH == 16
typedef	uint16				uintmax;
#else
typedef	uint8				uintmax;
#endif

typedef signed char			int8;
typedef signed short		int16;
typedef signed long			int32;
typedef signed long long	int64;
#if		CPU_ARCH == 64
typedef	int64				intmax;
#elif	CPU_ARCH == 32
typedef	int32				intmax;
#elif	CPU_ARCH == 16
typedef	int16				intmax;
#else
typedef	int8				intmax;
#endif

typedef float				float32;
typedef double				float64;
typedef long double			float128;
#if		CPU_ARCH == 64
typedef	float64				floatmax;
#else
typedef	float32				floatmax;
#endif

typedef uint8			uchar;
typedef uint16			ushort;
typedef unsigned int	uint;
typedef uint32			ulong;
typedef uint64			ullong;

typedef float128	ldouble;

typedef int8	schar;
typedef int64	llong;

typedef wchar_t	wchar;

typedef int8	sbyte;
typedef uint8	ubyte;
typedef int16	sword;
typedef uint16	uword;
typedef uint32	udword;
typedef uint64	uqword;

typedef ubyte	byte;
typedef uword	word;
typedef udword	dword;
typedef uqword	qword;

typedef size_t usize;
typedef ssize_t ssize;

#if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define CPU_X86
typedef __m128	uint128x86;
typedef __m128i	int128x86;
#endif
#if		CPU_ARCH == 64
typedef uintmax	uint128 __attribute__((vector_size(16)));
typedef uintmax	uint256 __attribute__((vector_size(32)));
typedef uintmax	uint512 __attribute__((vector_size(64)));

typedef intmax	int128 __attribute__((vector_size(16)));
typedef intmax	int256 __attribute__((vector_size(32)));
typedef intmax	int512 __attribute__((vector_size(64)));
#endif

#endif // CTL_EXTENDED_TYPES_H
