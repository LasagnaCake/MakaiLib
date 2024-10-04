#ifndef CTL_ALGORITHM_STRCONV_H
#define CTL_ALGORITHM_STRCONV_H

#include "../namespace.hpp"
#include "../container/string.hpp"

CTL_NAMESPACE_BEGIN

constexpr int		toInt(String const& s)				{return String::toNumber<int>(s);		}
constexpr long		toLong(String const& s)				{return String::toNumber<long>(s);		}
constexpr llong		toLongLong(String const& s)			{return String::toNumber<llong>(s);		}

constexpr uint		toUnsignedInt(String const& s)		{return String::toNumber<uint>(s);		}
constexpr ulong		toUnsignedLong(String const& s)		{return String::toNumber<ulong>(s);		}
constexpr ullong	toUnsignedLongLong(String const& s)	{return String::toNumber<ullong>(s);	}

constexpr float		toFloat(String const& s)		{return String::toNumber<float>(s);		}
constexpr double	toDouble(String const& s)		{return String::toNumber<double>(s);	}
constexpr ldouble	toLongDouble(String const& s)	{return String::toNumber<ldouble>(s);	}

inline int8		toInt8(String const& s)		{return String::toNumber<int8>(s);		}
inline int16	toInt16(String const& s)	{return String::toNumber<int16>(s);		}
inline int32	toInt32(String const& s)	{return String::toNumber<int32>(s);		}
inline int64	toInt64(String const& s)	{return String::toNumber<int64>(s);		}

inline uint8	toUInt8(String const& s)	{return String::toNumber<uint8>(s);		}
inline uint16	toUInt16(String const& s)	{return String::toNumber<uint16>(s);	}
inline uint32	toUInt32(String const& s)	{return String::toNumber<uint32>(s);	}
inline uint64	toUInt64(String const& s)	{return String::toNumber<uint64>(s);	}

inline float32	toFloat32(String s)		{return toFloat(s);			}
inline float64	toFloat64(String s)		{return toDouble(s);		}
inline float128	toFloat128(String s)	{return toLongDouble(s);	}

template<Type::Number T>
constexpr String toString(T const& value) {
	return String::fromNumber<T>(value);
}

template<class T>
constexpr String toString(T const& value)
requires (
	Type::Convertible<T, String>
&&	!Type::Number<T>
) {
	return String(value);
}

template<class T>
concept ClassStringable = requires (T t) {
	{t.toString()} -> Type::Equal<String>;
};

template<ClassStringable T>
constexpr String toString(T const& value) {
	return value.toString();
}

template<class T>
concept STDStringable = requires (T t) {
	{t.c_str()} -> Type::Equal<typename String::StringLiteralType>;
};

template<STDStringable T>
constexpr String toString(T const& value) {
	return value.c_str();
}

template<class T>
concept StringConvertible = requires (T t) {
	{toString(t)} -> Type::Equal<String>;
};

template<StringConvertible T, class... Args>
constexpr String toString(T const& first, Args const&... args)
	requires (... && StringConvertible<Args>) {
	return (... + toString(args));
}

template<class... Args>
constexpr WideString toWideString(Args const&... args)
requires (... && StringConvertible<Args>) {
	return toString(args...).toWideString();
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_STRCONV_H
