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

	constexpr Exception(ConstReferenceType message) noexcept: message(message), prev(ex) {
		ex = this;
	}

	constexpr Exception(SelfType const& other) noexcept: Exception(other.message) {}

	constexpr virtual char const* what() const noexcept {return message;}

	constexpr static Exception* current() noexcept 	{return ex;		}
	constexpr Exception* previous() const noexcept	{return prev;	}

private:
	char const* const message;

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

namespace {
	template<Type::Convertible<char const*> T>
	constexpr const char* cstring(T const& value) {
		return (const char*)value;
	}

	template<class T>
	constexpr const char* cstring(T const& value)
	requires requires (T t) {
		{t.cstr()} -> Type::Convertible<const char*>;
	} {
		return value.cstr();
	}

	template<class T>
	constexpr const char* cstring(T const& value)
	requires requires (T t) {
		{t.c_str()} -> Type::Convertible<const char*>;
	} {
		return value.c_str;
	}
}

template<typename T>
concept ErrorStringType =
	Type::Constructible<T, const char*>
&&	(Type::Constructible<T, T const&> || Type::Constructible<T, T>)
&&	Type::Addable<T, T>
&&	Type::Addable<T, const char*>
&&	Type::Addable<const char*, T>
&&	requires (T a, const char* b) {
		{cstring(a)};
		{a + a} -> Type::Equal<T>;
		{a + b} -> Type::Equal<T>;
		{b + a} -> Type::Equal<T>;
	}
;

template <ErrorStringType TString>
struct DetailedException:
	Exception,
	Derived<Exception>,
	Typed<TString>,
	SelfIdentified<DetailedException<TString>> {
public:
	using Typed				= ::CTL::Typed<TString>;
	using Derived			= ::CTL::Derived<Exception>;
	using SelfIdentified	= ::CTL::SelfIdentified<DetailedException<TString>>;

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
		typename BaseType::StringLiteralType
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
		BaseType(cstring(message)),
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
		bool,
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
			result = result
			+	"\n<stack>"
			+	previous()->what()
			+	"</stack>"
			;
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
		return cstring(sumbuf);
	}

private:
	DataType sumbuf = "";
};

CTL_NAMESPACE_END

#endif // CTL_CPPERROR_H
