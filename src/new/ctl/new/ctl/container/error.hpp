#ifndef CTL_CUSTOM_RUNTIME_ERRORS_H
#define CTL_CUSTOM_RUNTIME_ERRORS_H

#include "string.hpp"
#include "../cpperror.hpp"

CTL_NAMESPACE_BEGIN

namespace Error {
	using Generic = DetailedException<String>;

	#define DEFINE_ERROR_TYPE(NAME)\
		struct NAME: public Generic {\
			NAME (\
				String const& message		= "none",\
				String const& file			= "unspecified",\
				String const& line			= "unspecified",\
				String const& caller		= "unspecified",\
				String const& info			= "none",\
				String const& callerInfo	= "none"\
			): Generic (#NAME, message, file, line, caller, info, callerInfo) {}\
			\
			NAME (\
				bool,\
				Exception const& other,\
				String const& file			= "unspecified",\
				String const& line			= "unspecified",\
				String const& caller		= "unspecified",\
				String const& info			= "none",\
				String const& callerInfo	= "none"\
			): Generic (true, other, #NAME, file, line, caller, info, callerInfo) {}\
		}

	// "Invalid X" errors
	DEFINE_ERROR_TYPE(InvalidAction);
	DEFINE_ERROR_TYPE(InvalidValue);
	DEFINE_ERROR_TYPE(InvalidType);
	DEFINE_ERROR_TYPE(InvalidCall);
	DEFINE_ERROR_TYPE(InvalidCast);
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

	typedef Exception* ErrorPointer;

	ErrorPointer current() {return Exception::current();}

	template<Type::Derived<Generic> T>
	[[noreturn]] void rethrow(T const& err) {throw T(err);}

	template<Type::Derived<Generic> T>
	[[noreturn]] void rethrow(T* const& err) {if (err) throw T(&err);}

	template<Type::Derived<Generic> T>
	[[noreturn]] void rethrow(Exception const& err) {throw T(err);}

	template<Type::Derived<Generic> T>
	[[noreturn]] void rethrow(Exception* const& err) {throw T(&err);}
}

CTL_NAMESPACE_END

#define DEFINE_ERROR_TYPE(NAME)\
	struct NAME: public ::CTL::Error::Generic {\
		NAME (\
			CTL::String const& message		= "none",\
			CTL::String const& file			= "unspecified",\
			CTL::String const& line			= "unspecified",\
			CTL::String const& caller		= "unspecified",\
			CTL::String const& info			= "none",\
			CTL::String const& callerInfo	= "none"\
		): Generic (#NAME, message, file, line, caller, info, callerInfo) {}\
	}

#endif // CTL_CUSTOM_RUNTIME_ERRORS_H
