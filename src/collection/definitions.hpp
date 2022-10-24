#ifndef EXTRA_DEFINITION_MACROS_H
#define EXTRA_DEFINITION_MACROS_H

#define $func(args...)	[&](args) -> void

#define $addenum(NAME, STEP) \
    NAME##_$_$, \
    NAME = NAME##_$_$ + (STEP) - 1

#define $each(I, LIST) (auto& I: LIST)
#define $eachif(I, LIST, CONDITION) $each(I, LIST) if(CONDITION)

#define $iltf(A, B, C) (((A) > (B)) ? -(C) : +(C))

#define $range(I, BEGIN, END, STEP) (auto I = (BEGIN); I < (END); I += (STEP))
#define $ssrange(I, BEGIN, END) $range(I, BEGIN, END, $iltf(BEGIN, END, 1))

#define $enableif(COND)			std::enable_if<COND>::type
#define $derived(CLASS, BASE)	std::is_base_of<BASE, CLASS>::value

#define $isderivedof(CLASS, BASE) $enableif($derived(CLASS, BASE))

#endif // EXTRA_DEFINITION_MACROS_H
