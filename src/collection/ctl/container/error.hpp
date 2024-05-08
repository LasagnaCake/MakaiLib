#ifndef CTL_CUSTOM_RUNTIME_ERRORS_H
#define CTL_CUSTOM_RUNTIME_ERRORS_H

#include "string.hpp"
#include "../cpperror.hpp"

namespace Error {
	typedef DetailedError<String> Error;

	#define DEFINE_ERROR_TYPE(NAME)\
		struct NAME: public Error {\
			NAME (\
				string const& message		= "none",\
				string const& file			= "unspecified",\
				string const& line			= "unspecified",\
				string const& caller		= "unspecified",\
				string const& info			= "none",\
				string const& callerInfo	= "none"\
			): Error (#NAME, message, file, line, caller, info, callerInfo) {}\
		}

	// "Invalid X" errors
	DEFINE_ERROR_TYPE(InvalidAction);
	DEFINE_ERROR_TYPE(InvalidValue);
	DEFINE_ERROR_TYPE(InvalidType);
	// Value errors
	DEFINE_ERROR_TYPE(OutOfBounds);
	DEFINE_ERROR_TYPE(NonexistentValue);
	DEFINE_ERROR_TYPE(DuplicateValue);
	DEFINE_ERROR_TYPE(NullPointer);
	DEFINE_ERROR_TYPE(NotFound);
	// Other errors
	DEFINE_ERROR_TYPE(FailedAction);
	DEFINE_ERROR_TYPE(Unimplemented);
	DEFINE_ERROR_TYPE(UserIsAnIdiot);
	DEFINE_ERROR_TYPE(Other);
	DEFINE_ERROR_TYPE(NotAnError);

	#undef DEFINE_ERROR_TYPE

	typedef std::exception_ptr ErrorPointer;

	ErrorPointer current() {return std::current_exception();}

	[[noreturn]] void rethrow(ErrorPointer const& err) {std::rethrow_exception(err);}

	template<Type::Derived<Error> T>
	[[noreturn]] void rethrow(T const& err) {throw T(err);}
}

#define DEFINE_ERROR_TYPE(NAME)\
	struct NAME: public Error::Error {\
		NAME (\
			std::string const& message		= "none",\
			std::string const& file			= "unspecified",\
			std::string const& line			= "unspecified",\
			std::string const& caller		= "unspecified",\
			std::string const& info			= "none",\
			std::string const& callerInfo	= "none"\
		): Error (#NAME, message, file, line, caller, info, callerInfo) {}\
	}

#endif // CTL_CUSTOM_RUNTIME_ERRORS_H
