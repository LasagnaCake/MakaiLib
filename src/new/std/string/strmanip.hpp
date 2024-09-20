#ifndef MAKAILIB_STD_STRING_STRMANIP_H
#define MAKAILIB_STD_STRING_STRMANIP_H

#include "../cpptypes.hpp"

namespace Makai::STD {
	template<Type::Number T>
	using NumberFormat = Pair<T, size_t>;

	constexpr String strpad(String str, char const& chr, size_t const& width, bool const& left = true) {
		size_t const strsz = str.size();
		if(width > strsz)
			str.insert(left ? 0 : (strsz-1), width - strsz, chr);
		return str;
	}

	constexpr bool isHexString(String const& str) {
		return std::all_of(str.begin(), str.end(), [](unsigned char const& c)->bool{return std::isxdigit(c);});
	}

	constexpr String replace(String str, char const& c, char const& nc) {
		for (char& sc: str)
			if (sc == c) sc = nc;
		return str;
	}

	constexpr String replace(String str, List<char> const& chars, char const& nc) {
		for (char const& c: chars)
			str = replace(str, c, nc);
		return str;
	}

	constexpr String replace(String str, Arguments<char> const& chars, char const& nc) {
		for (char const& c: chars)
			str = replace(str, c, nc);
		return str;
	}

	struct CharacterReplacement {
		List<char>	targets;
		char		replacement;
	};

	constexpr String replace(String str, CharacterReplacement const& rep) {
		for (char const& c: rep.targets)
			str = replace(str, c, rep.replacement);
		return str;
	}

	constexpr String replace(String str, List<CharacterReplacement> const& reps) {
		for (CharacterReplacement const& rep: reps)
			str = replace (str, rep);
		return str;
	}

	constexpr String replace(String str, Arguments<CharacterReplacement> const& reps) {
		for (CharacterReplacement const& rep: reps)
			str = replace (str, rep);
		return str;
	}

	namespace Format {template<Type::Number T>
		inline String format(T const& val, size_t const& precision) {
			if (!precision)
				return std::to_string((long long)val);
			std::stringstream ss;
			Fold::strins(
				ss,
				std::fixed,
				std::setprecision(precision),
				val
			);
			return ss.str();
		}

		template<Type::Number T>
		inline String format(NumberFormat<T> const& fmt) {
			return format(fmt->first, fmt->second);
		}

		template<Type::Number T>
		constexpr String format(T const& val, size_t const& leading) {
			return pad(std::to_string(val), '0', leading);
		}

		template<Type::Number T>
		inline String format(T const& val, size_t const& precision, size_t const& leading) {
			return pad(format(val, precision), '0', leading);
		}
	}
}

#endif // MAKAILIB_STD_STRING_STRMANIP_H
