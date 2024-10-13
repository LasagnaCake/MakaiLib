#ifndef CTL_ALGORITHM_FORMAT_H
#define CTL_ALGORITHM_FORMAT_H

#include "../namespace.hpp"
#include "../container/string.hpp"
#include "../typetraits/traits.hpp"

CTL_NAMESPACE_BEGIN

namespace Format {
	enum class Justify {
		CFJ_LEFT,
		CFJ_RIGHT,
		CFJ_CENTER
	};

	constexpr String pad(String str, char const& chr, usize const& width, Justify const& just = Justify::CFJ_LEFT) {
		if(width > str.size()) {
			switch (just) {
				case Justify::CFJ_LEFT:		str.insert(chr, width - str.size(), 0);		break;
				case Justify::CFJ_RIGHT:	str.appendBack(width - str.size(), chr);	break;
				case Justify::CFJ_CENTER:
					usize lhs = usize((float(width) * 0.75) + 0.5);
					return pad(
						pad(
							str,
							chr,
							lhs,
							Justify::CFJ_LEFT
						),
						chr,
						width,
						Justify::CFJ_RIGHT
					);
				break;
			}
		}
		return str;
	}

	template <Type::Real T>
	constexpr String prettify(T const& num, usize const& precision, usize const& leading) {
		String strnum = String::fromNumber<T>(num, precision);
		return pad(strnum, '0', strnum.size() + leading);
	}

	template <Type::Integer T>
	constexpr String prettify(T const& num, usize const& precision, usize const& leading) {
		return prettify<floatmax>(num, precision, leading);
	}
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_FORMAT_H
