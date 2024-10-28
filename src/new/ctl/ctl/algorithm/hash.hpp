#ifndef CTL_ALGORITHM_HASH_H
#define CTL_ALGORITHM_HASH_H

#include "../ctypes.hpp"
#include "../namespace.hpp"
#include "../typetraits/traits.hpp"
#include "../typetraits/cast.hpp"

CTL_NAMESPACE_BEGIN

namespace Impl::Hash {
	// Based off of https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/libsupc%2B%2B/hash_bytes.cc
	namespace Simple {
		constexpr usize hash(const void* data, usize sz, usize seed) {
			const char* byte = static_cast<const char*>(data);
			for (; sz; --sz) {
				seed *= 131;
				seed += *byte++;
			}
			return seed;
		}
	}
	
	// Based off of https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/libsupc%2B%2B/hash_bytes.cc
	namespace FNV1a {
		consteval usize prime() {
			switch(sizeof(usize)) {
				case sizeof(uint64):	return static_cast<usize>(1099511628211ull);
				case sizeof(uint32):	return static_cast<usize>(16777619ull);
				default:				return 0;
			}
		}
		
		consteval usize offset() {
			switch(sizeof(usize)) {
				case sizeof(uint64):	return static_cast<usize>(14695981039346656037ull);
				case sizeof(uint32):	return static_cast<usize>(2166136261ull);
				default:				return 0;
			}
		}
		
		constexpr usize hash(void const* const& data, usize sz, usize seed = offset()) {
			const char* byte = static_cast<const char*>(data);
			for (; sz; --sz) {
				seed ^= static_cast<usize>(*byte++);
				seed *= prime();
			}
			return seed;
		}
	}
	
	// Based off of https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp
	namespace Murmur2 {
		constexpr void mix(usize& h, usize& k, usize const& m, int const& r) {
			k *= m;
			k ^= k >> r;
			k *= m;
			h *= m;
			h ^= k;
		}
		
		constexpr void shuffle(usize& h, usize const& m, usize const& v) {
			h ^= h >> v;
			h *= m;
			h ^= h >> v;
		}
		
		constexpr void shuffle(usize& h, usize const& m, usize const& v1, usize const& v2) {
			h ^= h >> v1;
			h *= m;
			h ^= h >> v2;
		}
		
		constexpr usize hash64(const void* data, usize sz, usize seed = 0) {
			constexpr usize	m = 0xc6a4a7935bd1e995ull;
			constexpr int	r = 47;
			usize s		= sz;
			usize hash	= seed;
			const usize* d1 = static_cast<usize const*>(data);
			const usize* end = d1 + (sz/8);
			while(data != end) {
				usize k = *d1++;
				mix(hash, k, m, r);
			}
			const byte* d2 = static_cast<const byte*>(data);
			usize t = 0;
			switch(sz & 7) {
			case 7:		t ^= static_cast<usize>(d2[6]) << 48;
			case 6:		t ^= static_cast<usize>(d2[5]) << 40;
			case 5:		t ^= static_cast<usize>(d2[4]) << 32;
			case 4:		t ^= static_cast<usize>(d2[3]) << 24;
			case 3:		t ^= static_cast<usize>(d2[2]) << 16;
			case 2:		t ^= static_cast<usize>(d2[1]) << 8;
			case 1:		t ^= static_cast<usize>(d2[0]);
			default:	break;
			}
			mix(hash, t, m, r);
			mix(hash, s, m, r);
			shuffle(hash, m, r);
			return hash;
		} 
		
		constexpr usize hash32(const void* data, usize sz, usize seed = 0) {
			constexpr usize	m = 0x5bd1e995;
			constexpr int	r = 24;
			usize s		= sz;
			usize hash	= seed;
			const byte* dt = static_cast<const byte*>(data);
			while(sz >= 4){
				usize k = *(usize*)dt;
				mix(hash, k, m, r);
				dt += 4;
				sz -= 4;
			}
			usize t = 0;
			switch(sz) {
			case 3:		t ^= dt[2] << 16;
			case 2:		t ^= dt[1] << 8;
			case 1:		t ^= dt[0];
			default:	break;
			};
			mix(hash, t, m, r);
			mix(hash, s, m, r);
			shuffle(hash, m, 13, 15);
			return hash;
		}
	}
	
	constexpr usize hash(const void* data, usize sz, usize seed = 0) {
		if constexpr (sizeof(usize) == sizeof(uint64))	
			return Murmur2::hash64(data, sz, seed);
		else if constexpr (sizeof(usize) == sizeof(uint32))
			return Murmur2::hash32(data, sz, seed);
		else return Simple::hash(data, sz, seed);
	}
}

struct Hasher {
	template <Type::Integer T>
	constexpr static usize hash(T const& value)			{return value;					}

	constexpr static usize hash(float const& value)		{return bitcast<uint32>(value);	}
	constexpr static usize hash(double const& value)	{return bitcast<uint64>(value);	}

	constexpr static usize hash(cstring const& str) {
		cstring s = str;
		while (*s++);
		return Impl::Hash::hash(str, s - str, s - str);
	}

	template <class T>
	constexpr static usize hash(T* const& value)		{return (usize)value;			}

	template <class T>
	constexpr static usize hash(T const& value) requires requires (T t) {
		{t.begin()};
		{t.end()};
	} {
		return Impl::Hash::hash(value.begin(), value.end() - value.begin(), value.end() - value.begin());
	}

	template <class T>
	constexpr static usize hash(T const& value) requires requires (T t) {
		{t.cbegin()};
		{t.cend()};
	} {
		return Impl::Hash::hash(value.cbegin(), value.cend() - value.cbegin(), value.cend() - value.cbegin());
	}

	template <class T>
	constexpr static usize hash(T const& value) requires requires (T t) {
		{t.data()};
		{t.size()};
	} {
		return Impl::Hash::hash(value.data(), value.size(), value.size());
	}

	template <class T>
	constexpr static usize hash(T const& value);
};

template<class TValue, class THasher>
concept Hashable = requires (TValue v) {
	{THasher::hash(v)} -> Type::Equal<usize>;
};

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_HASH_H
