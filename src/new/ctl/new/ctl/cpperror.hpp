#ifndef CTL_CPPERROR_H
#define CTL_CPPERROR_H

#include "namespace.hpp"
#include "staticvalue.hpp"
#include "templates.hpp"

CTL_NAMESPACE_BEGIN

struct Failure {};

struct CatastrophicFailure: Failure {};

struct AllocationFailure:	CatastrophicFailure {};
struct MaximumSizeFailure:	CatastrophicFailure {};

struct Exception;

struct Exception:
	Failure,
	SelfIdentified<Exception>,
	Typed<const char*>,
	StringLiterable<char> {
public:
	using Typed				= ::CTL::Typed<const char*>;
	using SelfIdentified	= ::CTL::SelfIdentified<Exception>;

	using
		typename Typed::DataType,
		typename Typed::ConstReferenceType
	;

	using
		typename SelfIdentified::SelfType
	;

	char const* const message;

	constexpr Exception(ConstReferenceType message) noexcept: message(message), prev(ex) {
		ex = this;
	}

	constexpr Exception(SelfType const& other) noexcept: Exception(other.message) {}

	constexpr virtual char const* what() const noexcept {return message;}

	constexpr static Exception* current() noexcept 	{return ex;		}
	constexpr Exception* previous() const noexcept	{return prev;	}

private:
	Exception* const prev		= nullptr;
	inline static Exception* ex	= nullptr;

	template <class TString> friend class DetailedError;
};

struct OutOfBoundsException:		Exception {using Exception::Exception;};
struct BadCallException:			Exception {using Exception::Exception;};
struct InvalidValueException:		Exception {using Exception::Exception;};
struct NonexistentValueException:	Exception {using Exception::Exception;};
struct InvalidConversionException:	Exception {using Exception::Exception;};
struct InvalidCastException:		Exception {using Exception::Exception;};
struct FailedActionException:		Exception {using Exception::Exception;};
struct MissingStreamException:		Exception {using Exception::Exception;};

template<typename T>
concept ErrorStringType =
	Type::Constructible<T, const char*>
&&	Type::Addable<T, T>
&&	Type::Addable<T, const char*>
&&	Type::Addable<const char*, T>
&&	Type::Convertible<T, const char*>
&&	Type::Addable<T, const char*>
&&	Type::Addable<const char*, T>
&&	Type::Addable<T, T>
&&	requires (T a, const char* b) {
		{a + a} -> Type::Equal<T>;
		{a + b} -> Type::Equal<T>;
		{b + a} -> Type::Equal<T>;
		{a += b} -> Type::Equal<T>;
	}
;

template <ErrorStringType TString>
struct DetailedException:
	Exception,
	Derived<Exception>,
	Typed<TString>,
	SelfIdentified<DetailedException<TString>>,
	StringLiterable<char> {
public:
	using Typed				= ::CTL::Typed<TString>;
	using Derived			= ::CTL::Derived<Exception>;
	using SelfIdentified	= ::CTL::SelfIdentified<DetailedException<TString>>;
	using StringLiterable	= ::CTL::StringLiterable<char>;

	using
		typename Typed::DataType,
		typename Typed::ConstReferenceType
	;

	using
		typename SelfIdentified::SelfType
	;

	using
		BaseType = typename Derived::Bases::FirstType
	;

	using
		typename StringLiterable::StringLiteralType
	;

	const DataType type;
	const DataType message;
	const DataType file;
	const DataType line;
	const DataType caller;
	const DataType info;
	const DataType callerInfo;

	constexpr DetailedException(
		ConstReferenceType type			= "Unknown",
		ConstReferenceType message		= "none",
		ConstReferenceType file			= "unspecified",
		ConstReferenceType line			= "unspecified",
		ConstReferenceType caller		= "unspecified",
		ConstReferenceType info			= "none",
		ConstReferenceType callerInfo	= "none"
	) noexcept:
		BaseType((const char*)message),
		type(type),
		message(message),
		file(file),
		line(line),
		caller(caller),
		info(info),
		callerInfo(callerInfo)
	{sumbuf = summary();}

	constexpr DetailedException(SelfType const& other) noexcept:
		SelfType(
			other.type,
			other.message,
			other.file,
			other.line,
			other.caller,
			other.info,
			other.callerInfo
		)
	{}

	constexpr DetailedException(
		BaseType const& other,
		ConstReferenceType type			= "Unknown",
		ConstReferenceType file			= "unspecified",
		ConstReferenceType line			= "unspecified",
		ConstReferenceType caller		= "unspecified",
		ConstReferenceType info			= "none",
		ConstReferenceType callerInfo	= "none"
	) noexcept:
		DetailedException(
			type,
			other.what(),
			file,
			line,
			caller,
			info,
			callerInfo
		)
	{}

	constexpr DataType report() const noexcept {
		DataType result = (
			"!!! AN ERROR HAS OCCURRED !!!\n\n"
			"<error>\n\n"
		+	type + ": " + message + "\n\n"
			"Caller: " + caller + "\n"
			"File: " + file + "\n"
			"Line: " + line + "\n"
			"\n[General Information]\n" + info + "\n"
			"\n[Caller Information]\n" + callerInfo + "\n"
		);
		if (previous()) {
			result += "\n<stack>";
			result += previous()->what();
			result += "</stack>";
		}
		result += "\n</error>";
		return result;
	}

	constexpr DataType summary() const noexcept {
		return
			(type + ": " + message + (!info.empty() ? ("\n" + info) : ""))
		+	(previous()
			?	"\n> " + DataType(previous()->what())
			:	""
			)
		;
	}

	constexpr StringLiteralType what() const noexcept override {
		return (StringLiteralType)sumbuf;
	}

private:
	DataType sumbuf = "";
};

CTL_NAMESPACE_END

#endif // CTL_CPPERROR_H
