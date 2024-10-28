#ifndef CTL_META_FOLD_H
#define CTL_META_FOLD_H

#include "../namespace.hpp"

CTL_NAMESPACE_BEGIN

namespace Fold {
	template<typename... Args>
	constexpr bool land(Args const&... args) {
		return (... && args);
	}

	template<typename... Args>
	constexpr bool lor(Args const&... args) {
		return (... || args);
	}

	template<typename T, typename... Args>
	constexpr T band(Args const&... args) {
		return (... & args);
	}

	template<typename T, typename... Args>
	constexpr T bor(Args const&... args) {
		return (... | args);
	}

	template<typename T, typename... Args>
	constexpr T bxor(Args const&... args) {
		return (... ^ args);
	}

	template<typename T, typename... Args>
	constexpr T add(Args const&... args) {
		return (... + args);
	}

	template<typename T, typename... Args>
	constexpr T sub(Args const&... args) {
		return (... - args);
	}

	template<typename T, typename... Args>
	constexpr T mul(Args const&... args) {
		return (... * args);
	}

	template<typename T, typename... Args>
	constexpr T div(Args const&... args) {
		return (... / args);
	}

	template<typename T, typename... Args>
	constexpr T& strins(T& stream, Args const&... args) {
		(stream << ... << args);
		return stream;
	}

	template<typename T, typename... Args>
	constexpr T avg(Args const&... args) {
		return (add(args...)) / sizeof...(Args);
	}
}

CTL_NAMESPACE_END

#endif // CTL_META_FOLD_H
