#ifndef TYPE_MAP_H
#define TYPE_MAP_H

#include "../conceptual.hpp"

template<class K, class V, Type::Integer I = size_t>
struct Map {
public:
	typedef K							KeyType;
	typedef V							ValueType;
	typedef Pair<KeyType, ValueType>	PairType;

	constexpr Map() {}

private:
};

#endif // TYPE_MAP_H
