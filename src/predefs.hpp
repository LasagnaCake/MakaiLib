#ifndef HEADER_PREDEFINITIONS_H
#define HEADER_PREDEFINITIONS_H

// GCC Attributes
#define METACTOR		[[gnu::constructor]]
#define METADTOR		[[gnu::destructor]]
#define SECTION(NAME)	[[gnu::section(NAME)]]
#define ALIGNED(V)		[[gnu::aligned(V)]]
// Declspecs
#define NOINLINE			__declspec(noinline)
#define MODULAR				__declspec(novtable)
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
#define UNAVAILABLE			[[unavailable]]
#define DEADEND				[[noreturn]]
#define KEEPRESULT			[[nodiscard]]
#define UNUSED				[[unused]]
#define MAYBEUNUSED			[[maybe_unused]]
#define FALLTHROUGH			[[fallthrough]]
#define LIKELY				[[likely]]
#define UNLIKELY			[[unlikely]]
// Types
#ifndef restrict
#define restrict __restrict__
#endif // restrict

#ifndef threadlocal
#define threadlocal thread_local
#endif // threadlocal

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
