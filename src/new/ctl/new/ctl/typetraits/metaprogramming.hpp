#ifndef CTL_TYPETRAITS_METAPROGRAMMING_H
#define CTL_TYPETRAITS_METAPROGRAMMING_H

namespace Meta {
	namespace Impl {
		template<bool COND, class True, class False>	struct DualType:						TypeContainer<True> {};
		template<class True, class False>				struct DualType<false, True, False>:	TypeContainer<False> {};
	}

	template <bool... Values>
	constexpr bool LogicalAnd	= (... && Values);

	template <bool... Values>
	constexpr bool LogicalOr	= (... && Values);

	template<bool COND, class True, class False>
	using DualType = Impl::DualType<COND, True, False>::type;
}

#endif // CTL_TYPETRAITS_METAPROGRAMMING_H
