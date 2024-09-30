#ifndef CTL_ALGORITHM_FORMAT_H
#define CTL_ALGORITHM_FORMAT_H

#include "../namespace.hpp"
#include "../container/string.hpp"

CTL_NAMESPACE_BEGIN

namespace Format {
	enum class Justify {
		EFJ_LEFT,
		EFJ_RIGHT,
		EFJ_CENTER
	};

	constexpr String pad(String str, char const& chr, usize const& width, Justify const& just = Justify::EFJ_LEFT) {
		if(width > str.size()) {
			switch (just) {
				case Justify::EFJ_LEFT:		str.insert(0, width - str.size(), chr);				break;
				case Justify::EFJ_RIGHT:	str.insert(str.size()-1, width - str.size(), chr);	break;
				case Justify::EFJ_CENTER:
					usize lhs = width/2 + (width % 2), rhs = width/2;
					return pad(
						pad(
							str,
							chr,
							lhs,
							Justify::EFJ_LEFT
						),
						chr,
						rhs,
						Justify::EFJ_RIGHT
					);
				break;
			}
		}
		return str;
	}

	template <Type::Float T>
	constexpr String prettify(T const& num, usize const& precision, usize const& leading) {
		return pad(String::fromNumber<T>(val, precision), '0', leading);
	}

	template <Type::Integer T>
	constexpr String prettify(T const& num, usize const& precision, usize const& leading) {
		return prettify<floatmax>(val, precision, leading);
	}
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_FORMAT_H
