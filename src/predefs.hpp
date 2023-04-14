#ifndef HEADER_PREDEFINITIONS_H
#define HEADER_PREDEFINITIONS_H

#define DLL_EXPORT			__declspec(dllexport)
#define DLL_IMPORT			__declspec(dllimport)
#define ALIGN(V)			__declspec(align(V))
#define ALLOCATOR			__declspec(allocator)
#define NOINLINE			__declspec(noinline)
#define MODULAR				__declspec(novtable)
#define SETONLY(SETFUNC)	__declspec(property(put=SETFUNC))
#define GETONLY(GETFUNC)	__declspec(property(get=GETFUNC))
#define SETGET(SET, GET)	__declspec(property(get=GET, put=SET))
#define ORPHANED			__declspec(restrict)
#define THREADLOCAL			__declspec(thread)
#define ASSUME(EXPR)		[[assume(EXPR)]]
#define DEPRECATED			[[deprecated]]
#define DEADEND				[[noreturn]]
#define FORCEKEEP			[[nodiscard]]

#ifndef restrict
#define restrict __restrict
#endif // restrict

#ifdef SHARED_LIBRARY
#ifdef COMPILING_DLL
#define SHAREDLIB DLL_EXPORT
#else
#define SHAREDLIB DLL_IMPORT
#endif // COMPILING_DLL
#else
#define SHAREDLIB
#endif // SHARED_LIBRARY

#endif // HEADER_PREDEFINITIONS_H
