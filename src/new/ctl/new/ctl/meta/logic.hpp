#ifndef CTL_META_LOGIC_H
#define CTL_META_LOGIC_H

#include "../namespace.hpp"

CTL_NAMESPACE_BEGIN

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

CTL_NAMESPACE_END

#endif // CTL_META_LOGIC_H
