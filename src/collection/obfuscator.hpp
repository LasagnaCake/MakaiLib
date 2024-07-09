#ifndef STRING_OBFUSCATION_H
#define STRING_OBFUSCATION_H

#include "helper.hpp"

namespace Obfuscation {
	template<class T, usize S>
	using Array = std::array<T, S>;

	template<class T, usize S>
	using CArray = Decay::AsType<T[S]>;

	template<usize S>
	using FixedCString = CArray<const char, S>;

	template<typename TData>
	struct Obfuscator {
		typedef TData DataType;

		constexpr virtual ~Obfuscator() {}

		virtual DataType deobfuscated() const = 0;

		DataType operator()() const	{return deobfuscated();}
		DataType operator*() const	{return deobfuscated();}
		operator DataType() const 	{return deobfuscated();}
	};

	namespace Impl {
		consteval bool isPrime(usize const& v) {
			for (usize i = 2; i < v; ++i)
				if (v % i == 0)
					return false;
			return true;
		}

		consteval usize nearestPrime(usize const& v, bool exclueSelf = false) {
			for (usize i = (exclueSelf ? (v-1) : v); i > 0; --i)
				if (nearestPrime(i))
					return i;
			return 0;
		}

		template<usize S, bool PARITY, usize MASK>
		struct MangledString;

		template<bool PARITY, usize MASK>
		struct MangledString<1, PARITY, MASK> {
			constexpr static usize SIZE = 1;

			constexpr MangledString()							{				}
			constexpr MangledString(FixedCString<1> const& dat)	{c = dat[0];	}
			constexpr MangledString(Array<uint8, 1> const& dat)	{c = dat[0];	}

			String build() const {return toString(c);}

		private:
			char c;
		};

		template<usize S, bool PARITY, usize MASK>
		struct MangledString {
			constexpr static usize SIZE = S;

			constexpr MangledString() {}

			constexpr MangledString(FixedCString<SIZE> const& dat) {
				decompose<FixedCString<SIZE>>(dat);
			}

			constexpr MangledString(Array<uint8, SIZE> const& dat) {
				decompose<Array<uint8, SIZE>>(dat);
			}

			String build() const requires (PARITY)	{return right.build() + left.build();}
			String build() const requires (!PARITY)	{return left.build() + right.build();}
		private:
			template<class TArray>
			constexpr void decompose(TArray const& dat) {
				Array<uint8, S1> s1;
				Array<uint8, S2> s2;
				for (usize i = 0; i < H2; ++i) {
					if constexpr (PARITY) {
						s1[i] = dat[i+H2];
						s2[i] = dat[i];
					} else {
						s1[i] = dat[i];
						s2[i] = dat[i+H2];
					}
				}
				if constexpr(H2 < H1) {
					if constexpr (PARITY)
						s1[H1-1] = dat[SIZE-1];
					else
						s2[H1-1] = dat[SIZE-1];
				}
				left	= MangledString<S1, LHS_PARITY, NEW_MASK>(s1);
				right	= MangledString<S2, RHS_PARITY, NEW_MASK>(s2);
			}

			constexpr static uint8 NEW_MASK = MASK ^ (MASK >> 2);

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
				S1{(PARITY) ? H1 : H2},
				S2{(PARITY) ? H2 : H1}
			;

			MangledString<S1, LHS_PARITY, NEW_MASK> left;
			MangledString<S2, RHS_PARITY, NEW_MASK> right;
		};
	}

	template<usize S>
	using MangledString = Impl::MangledString<S, true, 0b10110110>;

	template<typename T, usize S>
	concept StringContainer =
		Type::Constructible<T>
	&&	Type::Constructible<T, FixedCString<S> const&>
	&&	requires (T t) {
			{t.build()} -> Type::Equal<String>;
		}
	;

	template <usize N, template<usize> class TContainer = MangledString>
	struct ObfuscatedString: Obfuscator<String> {
		constexpr static usize SIZE = N+1;

		static_assert(StringContainer<TContainer<SIZE>, SIZE>, "Container is not a valid string container!");

		constexpr ObfuscatedString(FixedCString<SIZE> const& str): data(decompose(str)) {}

		constexpr virtual ~ObfuscatedString() {}

		DataType deobfuscated() const final {
			DataType result;
			uint8 off = 0;
			for(uint8 c : data.build())
				result.push_back(off += c);
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
}

using Obfuscation::ObfuscatedString;

#endif // STRING_OBFUSCATION_H
