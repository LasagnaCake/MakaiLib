#ifndef CTL_EX_CONTAINER_OBFUSCATOR_H
#define CTL_EX_CONTAINER_OBFUSCATOR_H

#include "../../ctl/ctl.hpp"
#include "../../ctl/exnamespace.hpp"

CTL_EX_NAMESPACE_BEGIN

/// @brief Decays to a "C-style" fixed array.
/// @tparam T Element type.
/// @tparam S Array size.
template<class T, usize S>
using CArray = Decay::AsType<T[S]>;

/// @brief Decays to a fixed-size "C-style" string.
/// @tparam S String size.
template<usize S>
using FixedCString = CArray<const char, S>;

/// @brief Container-related type constraints.
namespace Type::Ex::Container {
	/// @brief Type must be a valid static string obfuscator.
	template<typename T, usize S>
	concept StringObfuscator =
		::CTL::Type::Constructible<T>
	&&	::CTL::Type::Constructible<T, FixedCString<S> const&>
	&&	requires (T t) {
			{t.demangled()} -> ::CTL::Type::Equal<String>;
		}
	;
}

/// @brief Value obfuscator interface.
/// @tparam TData Value type.
template<typename TData>
struct Obfuscator: CTL::Typed<TData> {
	using Typed = CTL::Typed<TData>;

	using
		typename Typed::DataType
	;

	/// @brief Destructor.
	constexpr virtual ~Obfuscator() {}

	/// @brief Returns the deobfuscated value.
	/// @return Deobfuscated value.
	/// @note Must be implemented in the derived classes.
	virtual DataType deobfuscated() const = 0;

	/// @brief Returns the deobfuscated value.
	/// @return Deobfuscated value.
	DataType operator()() const	{return deobfuscated();}
	/// @brief Returns the deobfuscated value.
	/// @return Deobfuscated value.
	DataType operator*() const	{return deobfuscated();}
	/// @brief Returns the deobfuscated value.
	/// @return Deobfuscated value.
	operator DataType() const 	{return deobfuscated();}
};

/// @brief Implementations.
namespace Impl {
	/// @brief Returns whether a number is a prime.
	/// @param v Number to check.
	/// @return Whether it is prime.
	consteval bool isPrime(usize const& v) {
		if (v == 0) return false;
		for (usize i = 2; i < v; ++i)
			if (v % i == 0)
				return false;
		return true;
	}

	/// @brief Returns the nearest prime to a number, that is less than or equal to it.
	/// @param v Number to get nearest prime.
	/// @param excludeSelf Whether to exclude the number itself, if it is prime.
	/// @return Nearest prime.
	consteval usize nearestPrime(usize const& v, bool excludeSelf = false) {
		if (v < 2) return excludeSelf ? v : 0;
		for (usize i = (excludeSelf ? (v-1) : v); i > 0; --i)
			if (isPrime(i))
				return i;
		return 0;
	}

	/// @brief Contains information on the primality of a number.
	/// @tparam N Number to check.
	template<usize N>
	struct PrimeNumber {
		/// @brief Number.
		constexpr static usize VALUE	= N;
		/// @brief Whether it is prime.
		constexpr static bool IS_PRIME	= isPrime(N);
		/// @brief Nearest prime to it, excluding itself.
		constexpr static usize NEAREST	= nearestPrime(N, true);
	};
}

namespace StringMangler {
	template<usize S, usize MASK, bool PARITY>
	struct BinaryShuffle;

	template<usize MASK, bool PARITY>
	struct BinaryShuffle<1, MASK, PARITY> {
		constexpr static usize SIZE = 1;

		constexpr BinaryShuffle()							{				}
		constexpr BinaryShuffle(FixedCString<1> const& dat)	{c = dat[0];	}
		constexpr BinaryShuffle(Array<uint8, 1> const& dat)	{c = dat[0];	}

		String mangled() const		{return toString(c);}
		String demangled() const	{return toString(c);}

	private:
		char c;
	};

	template<usize S, usize M, bool P>
	struct BinaryShuffle {
		constexpr static usize
			SIZE = S,
			MASK = M
		;
		constexpr static bool PARITY = P;

		constexpr BinaryShuffle() {}

		constexpr BinaryShuffle(FixedCString<SIZE> const& dat) {
			decompose<FixedCString<SIZE>>(dat);
		}

		constexpr BinaryShuffle(Array<uint8, SIZE> const& dat) {
			decompose<Array<uint8, SIZE>>(dat);
		}

		String mangled() const						{return left.mangled() + right.mangled();		}

		String demangled() const requires (PARITY)	{return right.demangled() + left.demangled();	}
		String demangled() const requires (!PARITY)	{return left.demangled() + right.demangled();	}
	private:
		constexpr static usize NEW_MASK = MASK ^ (MASK >> 2);

		constexpr static bool
			EVEN_SIZE	= (SIZE % 2 == 0),
			LHS_PARITY	= (MASK & 0b10) ? !PARITY : PARITY,
			RHS_PARITY	= (MASK & 0b01) ? !PARITY : PARITY
		;

		constexpr static usize
			H1{(EVEN_SIZE) ? (SIZE/2) : (SIZE/2+1)},
			H2{(SIZE/2)}
		;

		constexpr static usize
			LEFT_SIZE{(PARITY) ? H1 : H2},
			RIGHT_SIZE{(PARITY) ? H2 : H1}
		;

		typedef BinaryShuffle<LEFT_SIZE, NEW_MASK, LHS_PARITY>	LeftType;
		typedef BinaryShuffle<RIGHT_SIZE, NEW_MASK, RHS_PARITY>	RightType;

		template<class TArray>
		constexpr void decompose(TArray const& dat) {
			Array<uint8, LEFT_SIZE>		l;
			Array<uint8, RIGHT_SIZE>	r;
			for (usize i = 0; i < H2; ++i) {
				if constexpr (PARITY) {
					l[i] = dat[i+H2];
					r[i] = dat[i];
				} else {
					l[i] = dat[i];
					r[i] = dat[i+H2];
				}
			}
			if constexpr(H2 < H1) {
				if constexpr (PARITY)
					l[H1-1] = dat[SIZE-1];
				else
					r[H1-1] = dat[SIZE-1];
			}
			left	= LeftType(l);
			right	= RightType(r);
		}

		LeftType	left;
		RightType	right;
	};

	template<usize S, usize MASK, bool PARITY>
	struct PrimeShuffle;

	template<usize MASK, bool PARITY>
	struct PrimeShuffle<1, MASK, PARITY> {
		constexpr static usize SIZE = 1;

		constexpr PrimeShuffle()							{				}
		constexpr PrimeShuffle(FixedCString<1> const& dat)	{c = dat[0];	}
		constexpr PrimeShuffle(Array<uint8, 1> const& dat)	{c = dat[0];	}

		String mangled() const		{return toString(c);}
		String demangled() const	{return toString(c);}

	private:
		char c;
	};

	template<usize S, usize M, bool P>
	struct PrimeShuffle {
		constexpr static usize
			SIZE = S,
			MASK = M
		;
		constexpr static bool PARITY = P;

		constexpr PrimeShuffle() {}

		constexpr PrimeShuffle(FixedCString<SIZE> const& dat) {
			decompose<FixedCString<SIZE>>(dat);
		}

		constexpr PrimeShuffle(Array<uint8, SIZE> const& dat) {
			decompose<Array<uint8, SIZE>>(dat);
		}

		String mangled() const						{return left.mangled() + right.mangled();		}

		String demangled() const requires (PARITY)	{return right.demangled() + left.demangled();	}
		String demangled() const requires (!PARITY)	{return left.demangled() + right.demangled();	}
	private:
		constexpr static usize NEW_MASK = MASK ^ (MASK >> 2);

		constexpr static bool
			LHS_PARITY	= (MASK & 0b10) ? !PARITY : PARITY,
			RHS_PARITY	= (MASK & 0b01) ? !PARITY : PARITY
		;

		constexpr static usize
			H1{Impl::nearestPrime(S, true)},
			H2{S - Impl::nearestPrime(S, true)}
		;

		constexpr static usize
			LEFT_SIZE{(PARITY) ? H1 : H2},
			RIGHT_SIZE{(PARITY) ? H2 : H1}
		;

		typedef PrimeShuffle<LEFT_SIZE, NEW_MASK, LHS_PARITY>	LeftType;
		typedef PrimeShuffle<RIGHT_SIZE, NEW_MASK, RHS_PARITY>	RightType;

		template<class TArray>
		constexpr void decompose(TArray const& dat) {
			Array<uint8, LEFT_SIZE>		l;
			Array<uint8, RIGHT_SIZE>	r;
			for (usize i = 0; i < LEFT_SIZE; ++i) {
				if constexpr (PARITY)	l[i] = dat[i+RIGHT_SIZE];
				else					l[i] = dat[i];
			}
			for (usize i = 0; i < RIGHT_SIZE; ++i) {
				if constexpr (PARITY)	r[i] = dat[i];
				else					r[i] = dat[i+LEFT_SIZE];
			}
			left	= LeftType(l);
			right	= RightType(r);
		}

		LeftType	left;
		RightType	right;
	};
}

template<usize S>
using MangledString = StringMangler::PrimeShuffle<S, Impl::PrimeNumber<S>::NEAREST, !Impl::PrimeNumber<S>::VALUE>;
//using MangledString = StringMangler::PrimeShuffle<S, 0b00010110, true>;
//using MangledString = StringMangler::BinaryShuffle<S, Impl::PrimeNumber<S>::NEAREST, !Impl::PrimeNumber<S>::VALUE>;
//using MangledString = StringMangler::BinaryShuffle<S, 0b00010110, true>;

template<usize S>
MangledString<S> makeMangled(FixedCString<S> const& str) {
	return MangledString<S>(str);
}

template <usize N, template<usize> class TContainer = MangledString>
struct ObfuscatedString: Obfuscator<String> {
	constexpr static usize SIZE = N+1;

	static_assert(
		Type::Ex::Container::StringObfuscator<TContainer<SIZE>, SIZE>,
		"Container is not a valid string obfuscator!"
	);

	constexpr ObfuscatedString(FixedCString<SIZE> const& str): data(decompose(str)) {}

	constexpr virtual ~ObfuscatedString() {}

	String deobfuscated() const final {
		String result;
		uint8 off = 0;
		for(uint8 c : data.demangled())
			result.pushBack(off += c);
		return result;
	}

private:
	typedef TContainer<SIZE> ContainerType;

	ContainerType data;

	constexpr static ContainerType decompose(FixedCString<SIZE> const& str) {
		Array<uint8, SIZE> result;
		uint8 off = 0;
		for (usize i = 0; i < SIZE; ++i) {
			result[i] = (str[i] - off);
			off = str[i];
		}
		return ContainerType(result);
	};
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_CONTAINER_OBFUSCATOR_H
