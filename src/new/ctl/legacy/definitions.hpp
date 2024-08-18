#ifndef EXTRA_DEFINITION_MACROS_H
#define EXTRA_DEFINITION_MACROS_H

#include "conceptual.hpp"
#include <cxxabi.h>

#define FUNC(...)	[&](__VA_ARGS__) -> void

#define ILTF(A, B, C) (((A) > (B)) ? -(C) : +(C))

// For loop range
#define RANGE(I, BEGIN, END, STEP) (auto I = (BEGIN); I < (decltype(I))(END); I += (decltype(I))(STEP))
// Single-stepping range
#define SRANGE(I, BEGIN, END) RANGE(I, BEGIN, END, ILTF(BEGIN, END, 1))

#define EACH(I, LIST) (auto& I: LIST)
#define EACH_IF(I, LIST, CONDITION) EACH(I, LIST) if(CONDITION)

#define SEACH(I, LIST, SIZE) SRANGE(_##I, 0, SIZE) { auto& I = LIST[_##I];
#define SEACH_IF(I, LIST, SIZE, CONDITION) SEACH(I, LIST, SIZE) if(CONDITION)
#define END_SEACH }

#define ERASE_IF(VEC, COND) if (!VEC.empty()) std::erase_if(VEC, [&](auto& elem){return (COND);})

#define NAMEOF(TYPEID) abi::__cxa_demangle((TYPEID).name(),0,0,NULL)

/*
#define DLL_EXPORT	__declspec(dllexport)
#define DLL_IMPORT	__declspec(dllimport)
*/
#endif // EXTRA_DEFINITION_MACROS_H
