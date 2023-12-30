#ifndef EXTRA_DEFINITION_MACROS_H
#define EXTRA_DEFINITION_MACROS_H

#include "conceptual.hpp"

#define FUNC(...)	[&](__VA_ARGS__) -> void

#define ILTF(A, B, C) (((A) > (B)) ? -(C) : +(C))

#define RANGE(I, BEGIN, END, STEP) (long long I = (BEGIN); I < (long long)(END); I += (STEP))
#define SSRANGE(I, BEGIN, END) RANGE(I, BEGIN, END, ILTF(BEGIN, END, 1))

#define EACH(I, LIST) (auto& I: LIST)
#define EACH_IF(I, LIST, CONDITION) EACH(I, LIST) if(CONDITION)

#define SEACH(I, LIST, SIZE) SSRANGE(_##I, 0, SIZE) { auto& I = LIST[_##I];
#define SEACH_IF(I, LIST, SIZE, CONDITION) SEACH(I, LIST, SIZE) if(CONDITION)
#define END_SEACH }

#define ERASE_IF(VEC, COND) if (!VEC.empty()) std::erase_if(VEC, [&](auto& elem){return (COND);})

#define NAMEOF(VALUE)	#VALUE
#define TYPENAME(VALUE)	typeid(VALUE).name()

/*
#define DLL_EXPORT	__declspec(dllexport)
#define DLL_IMPORT	__declspec(dllimport)
*/
#endif // EXTRA_DEFINITION_MACROS_H
