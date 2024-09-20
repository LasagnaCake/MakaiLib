#ifndef MAKAILIB_STD_DEBUG_H
#define MAKAILIB_STD_DEBUG_H

#ifdef MAKAILIB_DEBUG
#include <iostream>

namespace Makai::IO {
	template<typename... Args>
	inline void print(Args... args) {
		Fold::strins(std::cout, args...);
	}

	template<typename... Args>
	inline void println(Args... args) {
		print(args..., "\n");
	}

	template<typename... Args>
	inline void syncprint(Args... args) {
		Fold::strins(std::osyncstream(std::cout), args...);
	}

	template<typename... Args>
	inline void syncprintln(Args... args) {
		syncprint(args..., "\n");
	}
}

#ifndef DEBUG
#define DEBUG(...)		Makai::IO::print(__VA_ARGS__)
#endif // DEBUG
#ifndef DEBUGLN
#define DEBUGLN(...)	Makai::IO::println(__VA_ARGS__)
#endif // DEBUGLN
#ifndef SYNCDEBUG
#define SYNCDEBUG(...)		Makai::IO::syncprint(__VA_ARGS__)
#endif // SYNCDEBUG
#ifndef SYNCDEBUGLN
#define SYNCDEBUGLN(...)	Makai::IO::syncprintln(__VA_ARGS__)
#endif // SYNCDEBUGLN

#else

namespace Makai::IO {
	template<typename... Args>
	inline void print(Args... args) {}

	template<typename... Args>
	inline void println(Args... args) {}

	template<typename... Args>
	inline void syncprint(Args... args) {}

	template<typename... Args>
	inline void syncprintln(Args... args) {}
}

#ifndef DEBUG
#define DEBUG(...)
#endif // DEBUG
#ifndef DEBUGLN
#define DEBUGLN(...)
#endif // DEBUGLN
#ifndef SYNCDEBUG
#define SYNCDEBUG(...)
#endif // SYNCDEBUG
#ifndef SYNCDEBUGLN
#define SYNCDEBUGLN(...)
#endif // SYNCDEBUGLN

#endif // MAKAILIB_DEBUG

#endif // MAKAILIB_STD_DEBUG_H
