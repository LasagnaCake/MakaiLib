#ifndef CTL_ALGORITHM_STRCONV_H
#define CTL_ALGORITHM_STRCONV_H

#include "../namespace.hpp"
#include "../container/string.hpp"

CTL_NAMESPACE_BEGIN

constexpr int		toInt(String const& s, usize const& base = 10)				{return String::toNumber<int>(s, base);		}
constexpr long		toLong(String const& s, usize const& base = 10)				{return String::toNumber<long>(s, base);	}
constexpr llong		toLongLong(String const& s, usize const& base = 10)			{return String::toNumber<llong>(s, base);	}

constexpr uint		toUnsignedInt(String const& s, usize const& base = 10)		{return String::toNumber<uint>(s, base);	}
constexpr ulong		toUnsignedLong(String const& s, usize const& base = 10)		{return String::toNumber<ulong>(s, base);	}
constexpr ullong	toUnsignedLongLong(String const& s, usize const& base = 10)	{return String::toNumber<ullong>(s, base);	}

constexpr float		toFloat(String const& s)		{return String::toNumber<float>(s);		}
constexpr double	toDouble(String const& s)		{return String::toNumber<double>(s);	}
constexpr ldouble	toLongDouble(String const& s)	{return String::toNumber<ldouble>(s);	}

inline int8		toInt8(String const& s, usize const& base = 10)		{return String::toNumber<int8>(s, base);	}
inline int16	toInt16(String const& s, usize const& base = 10)	{return String::toNumber<int16>(s, base);	}
inline int32	toInt32(String const& s, usize const& base = 10)	{return String::toNumber<int32>(s, base);	}
inline int64	toInt64(String const& s, usize const& base = 10)	{return String::toNumber<int64>(s, base);	}

inline uint8	toUInt8(String const& s, usize const& base = 10)	{return String::toNumber<uint8>(s, base);	}
inline uint16	toUInt16(String const& s, usize const& base = 10)	{return String::toNumber<uint16>(s, base);	}
inline uint32	toUInt32(String const& s, usize const& base = 10)	{return String::toNumber<uint32>(s, base);	}
inline uint64	toUInt64(String const& s, usize const& base = 10)	{return String::toNumber<uint64>(s, base);	}

inline float32	toFloat32(String s)		{return toFloat(s);			}
inline float64	toFloat64(String s)		{return toDouble(s);		}
inline float128	toFloat128(String s)	{return toLongDouble(s);	}

template<Type::Number T>
constexpr String toString(T const& value) {
	return String::fromNumber<T>(value);
}

template<class T>
constexpr String toString(T const& value)
requires (Type::Convertible<T, String>) {
	return (value);
}

template<class T>
constexpr String toString(T const& value)
requires (T t) {
	requires T::toString;
	{t.toString()} -> Type::Equal<String>;
} {
	return value.toString();
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
