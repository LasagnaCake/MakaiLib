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

namespace Base {
	class CurrentException: private StaticValue<Exception*, nullptr> {
		friend class ::Exception;
	};
}

struct Exception:
	Failure,
	SelfIdentified<Exception>,
	Typed<const char*>,
	StringLiterable<char> {
public:
	using Typed				= Typed<const char*>;
	using SelfIdentified	= SelfIdentified<Exception>;

	using
		typename Typed::DataType,
		typename Typed::ConstReferenceType
	;

	using
		typename SelfIdentified::SelfType
	;

	char const* const message;

	constexpr Exception(ConstReferenceType _message) noexcept: message(_message), prev(ex) {
		ex = this;
	}

	constexpr Exception(SelfType const& other) noexcept: Exception(other.message) {}

	constexpr virtual char const* what() const noexcept {return message;}

	constexpr static Exception* current() noexcept	{return ex;		}
	constexpr static Exception* previous() noexcept	{return parent;	}

	constexpr bool isDetailed() noexcept	{return detailed;	}
	constexpr bool isSimple() noexcept		{return !detailed;	}

private:
	Exception* const prev		= nullptr;

	Base::CurrentException ex	= nullptr;

	template <class TString> friend class DetailedError;
};

struct OutOfRangeException:			Exception {using Exception::Exception;};
struct BadCallException:			Exception {using Exception::Exception;};
struct InvalidValueException:		Exception {using Exception::Exception;};
struct NonexistentValueException:	Exception {using Exception::Exception;};
struct InvalidConversionException:	Exception {using Exception::Exception;};
struct InvalidCastException:		Exception {using Exception::Exception;};

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
struct DetailedException:
	Exception,
	Derived<Exception>,
	Typed<TString>,
	SelfIdentified<DetailedException<TString>>,
	StringLiterable<char> {
public:
	using Typed				= Typed<TString>;
	using Derived			= Derived<Exception>;
	using SelfIdentified	= SelfIdentified<DetailedException<TString>>;
	using StringLiterable	= StringLiterable<char>;

	using
		DataType			= typename Typed::DataType,
		ConstReferenceType	= typename Typed::ConstReferenceType
	;

	using
		SelfType	= typename SelfIdentified::SelfType
	;

	using
		BaseType			= typename Derived::Bases::FirstType,
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
		detailed(true),
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
			+ "> "
			+ DataType(previous()->what())
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
