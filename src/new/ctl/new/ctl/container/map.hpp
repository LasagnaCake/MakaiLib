#ifndef CTL_CONTAINER_MAP_H
#define CTL_CONTAINER_MAP_H

#include "arguments.hpp"
#include "list.hpp"
#include "../namespace.hpp"
#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "../algorithm/hash.hpp"
#include "pair.hpp"

CTL_NAMESPACE_BEGIN

template<class TKey, class TValue, template<class TKey, class TValue> class TPair = Pair>
struct Collected {
	typedef Typed<TKey>			Key;
	typedef Typed<TValue>		Value;

	typedef TKey				KeyType;
	typedef TValue				ValueType;
	typedef TPair<TKey, TValue>	PairType;
};

template<class TKey, class TValue, Type::Integer TIndex = usize, class TPair = KeyValuePair<TKey, TValue>>
struct OrderedMap:
	List<TPair<TKey, TValue>, TIndex>,
	Collected<TKey, TValue, TPair>,
	SelfIdentified<OrderedMap<TKey, TValue, TIndex, TPair>>,
	Derived<List<TPair<TKey, TValue>, TIndex>>
requires (
	Type::Comparable::Threeway<TKey, TKey>
&&	PairType<TPair>
) {
public:
	using Derived			= Derived<List<TPair<TKey, TValue>, TIndex>>;
	using Collected			= Collected<TKey, TValue, TPair>;
	using SelfIdentified	= SelfIdentified<OrderedMap<TKey, TValue, TIndex, TPair>>;

	using BaseType = typename Derived::Bases::FirstType;

	using
		typename Collected::KeyType,
		typename Collected::ValueType,
		typename Collected::PairType
	;

	using typename SelfIdentified::SelfType;

	using
		typename BaseType::ArgumentListType,
		typename BaseType::SizeType,
		typename BaseType::IndexType
	;

	using BaseType::BaseType;

	constexpr ValueType at(KeyType const& key) const {
		if (empty()) return ValueType();
		PairType* start = cbegin();
		while(start != cend()) {
			if (start->key == key) return start->value;
			++start;
		}
		return ValueType();
	}

	constexpr ValueType& operator[](KeyType const& index) {
		if (empty()) return pushBack({key}).back().value;
		PairType* start = cbegin();
		while(start != cend()) {
			if (start->key == key) return start->value;
			++start;
		}
		return pushBack({key}).back().value;
	}

	constexpr ValueType operator[](KeyType const& index) const	{return at(index);}

	constexpr bool contains(KeyType const& key) const {
		if (empty()) return false;
		for(ValueType& e: (*this))
			if (e.key == key) return true;
		return false;
	}

private:
};

template<class TKey, class TValue, Type::Integer TIndex = usize, class TPair = KeyValuePair<TKey, TValue>>
struct SimpleMap:
	Collected<TKey, TValue, TPair>,
	Derived<List<TPair<TKey, TValue>, TIndex>>,
	SelfIdentified<SimpleMap<TKey, TValue, TIndex, TPair>>,
	private List<TPair<TKey, TValue>, TIndex>
requires (
	Type::Comparable::Threeway<TKey, TKey>
&&	PairType<TPair>
) {
public:
	using Derived			= Derived<List<TPair<TKey, TValue>, TIndex>>;
	using Collected			= Collected<TKey, TValue, TPair>;
	using SelfIdentified	= SelfIdentified<OrderedMap<TKey, TValue, TIndex, TPair>>;

	using BaseType = typename Derived::Bases::FirstType;

	using
		typename Collected::KeyType,
		typename Collected::ValueType,
		typename Collected::PairType
	;

	using typename SelfIdentified::SelfType;

	using
		typename BaseType::ArgumentListType,
		typename BaseType::SizeType,
		typename BaseType::IndexType,
		typename BaseType::IteratorType,
		typename BaseType::ReverseIteratorType
	;

	using
		BaseType::~BaseType,
		BaseType::tighten,
		BaseType::clear,
		BaseType::dispose,
		BaseType::operator==,
		BaseType::operator<=>,
		BaseType::equals,
		BaseType::compare,
		BaseType::disparity,
		BaseType::data,
		BaseType::begin,
		BaseType::end,
		BaseType::cbegin,
		BaseType::cend,
		BaseType::rbegin,
		BaseType::rend,
		BaseType::sort,
		BaseType::size,
		BaseType::capacity,
		BaseType::empty
	;

	constexpr SimpleMap(): BaseType() {}

	constexpr SimpleMap(SizeType const& size): BaseType(size) {}

	constexpr SimpleMap(SizeType const& size, DataType const& fill): BaseType(size, fill) {}

	constexpr SimpleMap(ArgumentListType const& values): BaseType(values) {sort();}

	template<SizeType N>
	constexpr SimpleMap(DataType const(& values)[N]): BaseType<N>(values) {sort();}

	constexpr SimpleMap(BaseType const& other): BaseType(other) {sort();}

	constexpr SimpleMap(BaseType&& other): BaseType(move(other)) {sort();}

	constexpr SimpleMap(SelfType const& other): BaseType(other) {}

	constexpr SimpleMap(SelfType&& other): BaseType(move(other)) {}

	constexpr SimpleMap(IteratorType const& begin, IteratorType const& end): BaseType(begin, end) {sort();}

	constexpr SimpleMap(ReverseIteratorType const& begin, ReverseIteratorType const& end): BaseType(begin, end) {sort();}

	constexpr ValueType at(KeyType const& key) const {
		if (empty()) return pushBack({key}).back().value;
		IndexType i = find(index);
		if (i == -1)	return ValueType();
		else			return data()[i].value;
	}

	constexpr ValueType& operator[](KeyType const& key) {
		if (empty()) return pushBack({key}).back().value;
		IndexType i = find(key);
		if (i == -1) {
			pushBack({key}).sort();
			return data()[find(key)].value;
		} else return data()[i].value;
	}

	constexpr IndexType find(KeyType const& value) const {
		if (empty()) return -1;
		IndexType pivot = count / 2, index = pivot;
		while (pivot != 0) {
			switch (data()[index].key <=> key) {
				case OrderType::EQUAL:		return index;
				case OrderType::GREATER:	index -= (pivot /= 2);	break;
				case OrderType::LESS:		index += (pivot /= 2);	break;
				case OrderType::UNORDERED:	return -1;
			}
		}
		return -1;
	}

	constexpr ValueType operator[](KeyType const& index) const	{return at(index);}

	constexpr bool contains(KeyType const& key) const {
		if (empty()) return false;
		return find(key) != -1;
	}

	constexpr SelfType& remove(KeyType const& key) {
		IndexType i = find(key);
		if (i == -1) return *this;
		BaseType::remove(i);
		return *this;
	}

	constexpr SelfType& removeIf(Function<bool(PairType const&)> const& predicate) {
		BaseType::removeIf(predicate);
		return *this;
	}

	constexpr SelfType& erase(KeyType const& key) {
		IndexType i = find(key);
		if (i == -1) return *this;
		BaseType::erase(i);
		return *this;
	}

	constexpr SelfType& eraseIf(Function<bool(PairType const&)> const& predicate) {
		BaseType::eraseIf(predicate);
		return *this;
	}

	constexpr SelfType& insert(PairType const& pair) {
		BaseType::pushBack(pair).sort();
		return *this;
	}

	constexpr SelfType& append(SelfType const& other) {
		BaseType::appendBack(other).sort();
		return *this;
	}

	constexpr SelfType& append(ArgumentListType const& values) {
		BaseType::appendBack(values).sort();
		return *this;
	}

	constexpr SelfType& append(SizeType const& count, DataType const& fill) {
		BaseType::appendBack(count, fill).sort();
		return *this;
	}

	constexpr SelfType& append(IteratorType const& begin, IteratorType const& end) {
		BaseType::appendBack(begin, end).sort();
		return *this;
	}

	constexpr SelfType& append(ReverseIteratorType const& begin, ReverseIteratorType const& end) {
		BaseType::appendBack(begin, end).sort();
		return *this;
	}

	template<SizeType S>
	constexpr SelfType& append(DataType const(& values)[S]) {
		BaseType::appendBack<S>(values).sort();
		return *this;
	}

private:
};

/*
template<class TKey, class TValue, Type::Integer TIndex = usize, class TPair = Pair<TKey, TValue>, class THasher = Hasher>
struct HashMap:
	List<TPair<TKey, TValue>, TIndex>,
	Collected<TKey, TValue, TPair>,
	SelfIdentified<HashMap<TKey, TValue, TIndex, TPair>>
requires (
	Type::Comparable::Threeway<TKey, TKey>
&&	PairType<TPair>
&&	Hashable<TKey, THasher>
) {

}
*/

template<class TKey, class TValue>
using Map	= SimpleMap<TKey, TValue>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_MAP_H
