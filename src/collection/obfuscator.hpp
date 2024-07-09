#ifndef STRING_OBFUSCATION_H
#define STRING_OBFUSCATION_H

#include "helper.hpp"

namespace Obfuscation {
	template<class T, usize S>
	using Array = std::array<T, S>;

	template<class T, usize S>
	using CArray = Decay::AsType<T[S]>;

	template<usize S>
	using FixedString = Array<const char, S>;

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
		template<usize S>
		struct StringNode;

		template<>
		struct StringNode<1> {
			constexpr StringNode()								{				}
			constexpr StringNode(FixedCString<1> const& dat)	{c = dat[0];	}
			constexpr StringNode(Array<uint8, 1> const& dat)	{c = dat[0];	}

			String build() const {return toString(c);}

		private:
			char c;
		};

		template<usize S>
		struct StringNode {
			constexpr StringNode() {}

			constexpr StringNode(FixedCString<S> const& dat) {
				Array<uint8, S1> s1;
				Array<uint8, S2> s2;
				for (usize i = 0; i < S2; ++i) {
					s1[i] = dat[i+S2];
					s2[i] = dat[i];
				}
				if constexpr(S2 < S1)
					s1[S1-1] = dat[S-1];
				left = StringNode<S1>(s1);
				right = StringNode<S2>(s2);
			}

			constexpr StringNode(Array<uint8, S> const& dat) {
				Array<uint8, S1> s1;
				Array<uint8, S2> s2;
				for (usize i = 0; i < S2; ++i) {
					s1[i] = dat[i+S2];
					s2[i] = dat[i];
				}
				if constexpr(S2 < S1)
					s1[S1-1] = dat[S-1];
				left = StringNode<S1>(s1);
				right = StringNode<S2>(s2);
			}

			String build() const {return right.build() + left.build();}
		private:
			constexpr static usize S1{(S % 2 == 0) ? (S/2) : (S/2+1)};
			constexpr static usize S2{(S/2)};

			StringNode<S1> left;
			StringNode<S2> right;
		};
	}

	template <usize N>
	struct ObfuscatedString: Obfuscator<String> {
		constexpr static usize STRING_SIZE = N+1;

		constexpr ObfuscatedString(FixedCString<STRING_SIZE> const& str): head(decompose(str)) {}

		constexpr virtual ~ObfuscatedString() {}

		DataType deobfuscated() const final {
			DataType result;
			uint8 off = 0;
			for(uint8 c : head.build())
				result.push_back(off += c);
			return result;
		}

	private:
		typedef Impl::StringNode<STRING_SIZE> StringWrapper;

		StringWrapper head;

		constexpr static StringWrapper decompose(FixedCString<STRING_SIZE> const& str) {
			Array<uint8, STRING_SIZE> result;
			uint8 off = 0;
			for (usize i = 0; i < STRING_SIZE; ++i) {
				result[i] = (str[i] - off);
				off = str[i];
			}
			return StringWrapper(result);
		};
	};
}

using Obfuscation::ObfuscatedString;

#endif // STRING_OBFUSCATION_H
