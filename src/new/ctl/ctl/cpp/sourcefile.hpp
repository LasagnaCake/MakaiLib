#ifndef CTL_CPP_SOURCEFILE_H
#define CTL_CPP_SOURCEFILE_H

#include "../namespace.hpp"

#include "../container/string.hpp"
#include "../algorithm/strconv.hpp"

CTL_NAMESPACE_BEGIN

/// @brief Facilites pertaining to C++ source files.
namespace CPP {
	/// @brief Source file information.
	struct SourceFile {
		/// @brief Constructs the information structure. No values are required to be passed.
		/// @param function Function name.
		/// @param line Source file line.
		/// @param file Source file.
		constexpr SourceFile(
			String const&	function	= __builtin_FUNCTION(),
			int const		line		= __builtin_LINE(),
			String const&	file		= __builtin_FILE()
		): function(function), file(file), line(line) {}

		String lineName() const {
			if (line > 1) return toString(line);
			return "unspecified";
		}

		String const	function;
		String const	file;
		int const		line;
	};
}

CTL_NAMESPACE_END

#define CTL_CPP_BASIC_SOURCE (::CTL::CPP::SourceFile())
#define CTL_CPP_PRETTY_SOURCE (::CTL::CPP::SourceFile(__PRETTY_FUNCTION__))
#define CTL_CPP_UNKNOWN_SOURCE (::CTL::CPP::SourceFile("unknown", -1))

#define CTL_CPP_DEFAULT_SOURCE (CTL_CPP_BASIC_SOURCE)

#endif