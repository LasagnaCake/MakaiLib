#ifndef CTL_REGEX_CORE_H
#define CTL_REGEX_CORE_H

#include "../namespace.hpp"
#include "../container/string.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmismatched-new-delete"
#include <regex>
#pragma GCC diagnostic pop

CTL_NAMESPACE_BEGIN

namespace Regex {
	namespace {
		inline std::string stdstr(String const& expr) {
			return expr.std();
		}

		inline String ctlstr(std::string const& expr) {
			return String(expr);
		}

		inline std::regex makeRegex(String const& expr) {
			return std::regex(stdstr(expr));
		}
	}

	inline String replace(String const& str, String const& expr, String const& fmt) {
		return std::regex_replace(stdstr(str), makeRegex(expr), stdstr(fmt)).c_str();
	}

	inline bool contains(String const& str, String const& expr) {
		std::smatch rm;
		auto cs = stdstr(str);
		return std::regex_search(cs, rm, makeRegex(expr));
	}

	inline bool matches(String const& str, String const& expr) {
		std::smatch rm;
		auto cs = stdstr(str);
		return std::regex_match(cs, rm, makeRegex(expr));
	}

	struct Match {
		ssize	position;
		String	match;
	};

	inline List<Match> find(String const& str, String const& expr) {
		std::smatch rm;
		List<Match> result;
		auto cs = stdstr(str);
		std::regex_search(cs, rm, makeRegex(expr));
		for (usize i = 1; i < rm.size()-1; ++i)
			result.pushBack(Match{
				rm.position(i),
				ctlstr(rm[i].str())
			});
		return result;
	}

	inline Match findFirst(String const& str, String const& expr) {
		std::smatch rm;
		auto cs = stdstr(str);
		std::regex_search(cs, rm, makeRegex(expr));
		return Match{rm.position(1), ctlstr(rm[1].str())};
	}
}

CTL_NAMESPACE_END

#endif // CTL_REGEX_CORE_H
