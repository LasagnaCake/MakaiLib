#ifndef CUSTOM_RUNTIME_ERRORS_H
#define CUSTOM_RUNTIME_ERRORS_H

#include "templates.hpp"

class BasicError {
public:
	const char* const message;

	constexpr BasicError(const char* const& _message): message(_message) {}

	constexpr BasicError(BasicError const& other): BasicError(other.message) {}

	constexpr const char* what() const noexcept {return message;}
};

template<typename T>
concept ErrorStringType =
	Type::Constructible<T, const char*>
&&	Type::Addable<T, T>
&&	Type::Addable<T, const char*>
&&	Type::Addable<const char*, T>
&&	Type::Convertible<T, const char*>
&&	requires (T a, const char* b) {
		{a + a} -> Type::Convertible<T>;
		{a + b} -> Type::Convertible<T>;
		{b + a} -> Type::Convertible<T>;
	}
;

template <ErrorStringType TString>
class DetailedError: public BasicError, public Typed<TString> {
public:
	const DataType type;
	const DataType message;
	const DataType file;
	const DataType line;
	const DataType caller;
	const DataType info;
	const DataType callerInfo;

	constexpr DetailedError(
		DataType const& _type		= "Error",
		DataType const& _message	= "none",
		DataType const& _file		= "unspecified",
		DataType const& _line		= "unspecified",
		DataType const& _caller		= "unspecified",
		DataType const& _info		= "none",
		DataType const& _callerInfo	= "none"
	):
		BasicError(_message),
		type(_type),
		message(_message),
		file(_file),
		line(_line),
		caller(_caller),
		info(_info),
		callerInfo(_callerInfo)
	{sumbuf = summary();}

	constexpr DetailedError(Error const& other):
		DetailedError(
			other.type,
			other.message,
			other.file,
			other.line,
			other.caller,
			other.info,
			other.callerInfo
		)
	{}

	constexpr DataType report() const noexcept {
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

	constexpr DataType summary() const noexcept {
		return (type + ": " + message + (!info.empty() ? ("\n" + info) : ""));
	}

	constexpr const char* what() const noexcept {
		return (const char*)sumbuf;
	}

private:
	DataType sumbuf = "";
};

#endif // CUSTOM_RUNTIME_ERRORS_H
