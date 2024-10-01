#ifndef CTL_IO_CONSOLE_H
#define CTL_IO_CONSOLE_H

#include "../namespace.hpp"
#include "../meta/fold.hpp"
#include "../cpperror.hpp"
#include "stream.hpp"

#ifdef CTL_CONSOLE_OUT
#include <iostream>
#endif // CTL_CONSOLE_OUT

CTL_NAMESPACE_BEGIN

struct Console {
	#ifdef CTL_CONSOLE_OUT
	inline static InputStream<char>		in	= InputStream<char>(std::cin.rdbuf());
	inline static OutputStream<char>	out	= OutputStream<char>(std::cout.rdbuf());
	#else
	inline static InputStream<char>		in	= InputStream<char>(nullptr);
	inline static OutputStream<char>	out	= OutputStream<char>(nullptr);
	#endif // CTL_CONSOLE_OUT

	template<typename... Args>
	static void print(Args... args) {
		Fold::strins(out, args...);
	}

	template<typename... Args>
	static void println(Args... args) {
		print(args..., "\n");
	}

	static void flush() {
		print(std::endl);
	}

	template<typename... Args>
	static void syncprint(Args... args) {
		Fold::strins(std::osyncstream(out), args...);
	}

	template<typename... Args>
	static void syncprintln(Args... args) {
		syncprint(args..., "\n");
	}

	static void syncflush() {
		syncprint(std::endl);
	}

	template<class T>
	static T get() {
		if (!in)
			throw MissingStreamException("No console input/output stream was attached!");
		T val;
		in >> val;
		return val;
	}

	template<class T>
	static T syncget() {
		if (!in)
			throw MissingStreamException("No console input/output stream was attached!");
		T val;
		in.sync();
		in >> val;
		return val;
	}
};

CTL_NAMESPACE_END

#ifdef CTL_CONSOLE_OUT
#define DEBUG(...)			CTL::Console::print(__VA_ARGS__)
#define DEBUGLN(...)		CTL::Console::println(__VA_ARGS__)
#define SYNCDEBUG(...)		CTL::Console::syncprint(__VA_ARGS__)
#define SYNCDEBUGLN(...)	CTL::Console::syncprintln(__VA_ARGS__)
#else
#endif // CTL_CONSOLE_OUT

#endif // CTL_IO_CONSOLE_H
