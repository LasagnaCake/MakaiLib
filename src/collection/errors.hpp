#ifndef CUSTOM_RUNTIME_ERRORS_H
#define CUSTOM_RUNTIME_ERRORS_H

#include <string>
#include <stdexcept>
#include "conceptual.hpp"

namespace Error {
	namespace {
		using
			std::runtime_error,
			std::string
		;
	}

	class Error: public runtime_error {
	public:
		const string type;
		const string message;
		const string file;
		const string line;
		const string caller;
		const string info;
		const string callerInfo;

		Error(
			string const& _type			= "Error",
			string const& _message		= "none",
			string const& _file			= "unspecified",
			string const& _line			= "unspecified",
			string const& _caller		= "unspecified",
			string const& _info			= "none",
			string const& _callerInfo	= "none"
		):
			runtime_error(_message),
			type(_type),
			message(_message),
			file(_file),
			line(_line),
			caller(_caller),
			info(_info),
			callerInfo(_callerInfo)
		{sumbuf = summary();}

		Error(Error const& other):
			Error(
				other.type,
				other.message,
				other.file,
				other.line,
				other.caller,
				other.info,
				other.callerInfo
			)
		{}

		string report() const noexcept {
			return (
				"!!! AN ERROR HAS OCCURRED !!!\n\n"
				"<error>\n\n"
			+	type + ": " + message + "\n\n"
				"Caller: " + caller + "\n"
				"File: " + file + "\n"
				"Line: " + line + "\n"
				"\n[General Information]\n" + info + "\n"
				"\n[Caller Information]\n" + callerInfo + "\n"
				"\n</error>"
			);
		}

		string summary() const noexcept {
			return (type + ": " + message + (!info.empty() ? ("\n" + info) : "")).c_str();
		}

		const char* what() const noexcept {
			return sumbuf.c_str();
		}

	private:
		string sumbuf = "";
	};

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

#endif // CUSTOM_RUNTIME_ERRORS_H
