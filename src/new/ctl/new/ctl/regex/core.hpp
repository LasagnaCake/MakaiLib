#ifndef CTL_REGEX_CORE_H
#define CTL_REGEX_CORE_H

#include "../namespace.hpp"
#include "../container/string.hpp"
#include <regex>

CTL_NAMESPACE_BEGIN

namespace Regex {
	inline String replace(String const& str, String const& expr, String const& fmt) {
		return std::regex_replace(str, makeRegex(expr), fmt).c_str();
	}

	inline bool contains(String const& str, String const& expr) {
		std::smatch rm;
		return std::regex_search(str, rm, makeRegex(expr));
	}

	inline bool matches(String const& str, String const& expr) {
		std::smatch rm;
		return std::regex_match(str, rm, makeRegex(expr));
	}

	struct Match {
		usize	position;
		String	match;
	};

	inline List<String> find(String const& str, String const& expr) {
		std::smatch rm;
		List<String> result;
		std::regex_search(str, rm, makeRegex(expr));
		for (usize i = 1; i < rm.size()-1; ++i)
			result.pushBack({
				rm.position(i),
				rm[i].str().c_str()
			});
		return result;
	}

	inline String findFirst(String const& str, String const& expr) {
		std::smatch rm;
		std::regex_search(str, rm, makeRegex(expr));
		return rm[1].str().c_str();
	}
}

CTL_NAMESPACE_END

#endif // CTL_REGEX_CORE_H
