#ifndef HEADER_PREDEFINITIONS_H
#define HEADER_PREDEFINITIONS_H

#ifdef MSVCPP
#define THREADLOCAL			__declspec(thread)
#define ALIGN(V)			__declspec(align(V))
#define NOINLINE			__declspec(noinline)
#define MODULAR				__declspec(novtable)
#else
#define THREADLOCAL			__thread
#define ALIGN(V)            __attribute__((aligned(V)))
#define NOINLINE			__attribute__((noinline))
#define MODULAR				__attribute__((novtable))
#endif
// declspecs
#define ALLOCATOR			__declspec(allocator)
#define SETONLY(SETFUNC)	__declspec(property(put=SETFUNC))
#define GETONLY(GETFUNC)	__declspec(property(get=GETFUNC))
#define SETGET(SET, GET)	__declspec(property(get=GET, put=SET))
#define ORPHANED			__declspec(restrict)
#define DLL_EXPORT			__declspec(dllexport)
#define DLL_IMPORT			__declspec(dllimport)
// C++ Attributes
#define DEPENDANT			[[carries_dependency]]
#define ASSUME(EXPR)		[[assume(EXPR)]]
#define DEPRECATED			[[deprecated]]
#define DEADEND				[[noreturn]]
#define FORCEKEEP			[[nodiscard]]
#define UNUSED				[[unused]]
// Pragmas
//#define DERIVEONLY		_Pragma("interface")
// Types
#ifndef restrict
#define restrict __restrict__
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
