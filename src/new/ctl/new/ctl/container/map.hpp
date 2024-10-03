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

template<class TKey, class TValue, template <class TPairKey, class TPairValue> class TPair = Pair>
struct Collected {
	typedef Typed<TKey>			Key;
	typedef Typed<TValue>		Value;

	typedef TKey				KeyType;
	typedef TValue				ValueType;
	typedef TPair<TKey, TValue>	PairType;
};

template<class TKey, class TValue, Type::Integer TIndex = usize>
struct OrderedMap:
	List<KeyValuePair<TKey, TValue>, TIndex>,
	Collected<TKey, TValue, KeyValuePair>,
	SelfIdentified<OrderedMap<TKey, TValue, TIndex>>,
	Derived<List<KeyValuePair<TKey, TValue>, TIndex>> {
public:
	static_assert(Type::Comparable::Threeway<TKey, TKey>, "Cannot form a map whithout an orderable key!");

	using Derived			= ::CTL::Derived<List<KeyValuePair<TKey, TValue>, TIndex>>;
	using Collected			= ::CTL::Collected<TKey, TValue, KeyValuePair>;
	using SelfIdentified	= ::CTL::SelfIdentified<OrderedMap<TKey, TValue, TIndex>>;

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

	using
		BaseType::BaseType,
		BaseType::empty,
		BaseType::size,
		BaseType::cbegin,
		BaseType::cend
	;

	constexpr ValueType at(KeyType const& key) const {
		if (empty()) return ValueType();
		PairType* start = cbegin();
		while(start != cend()) {
			if (start->key == key) return start->value;
			++start;
		}
		return ValueType();
	}

	constexpr ValueType& operator[](KeyType const& key) {
		if (empty()) return pushBack({key}).back().value;
		PairType* start = cbegin();
		while(start != cend()) {
			if (start->key == key) return start->value;
			++start;
		}
		return pushBack({key}).back().value;
	}

	constexpr ValueType operator[](KeyType const& key) const	{return at(key);}

	constexpr bool contains(KeyType const& key) const {
		if (empty()) return false;
		for(ValueType& e: (*this))
			if (e.key == key) return true;
		return false;
	}

private:
};

template<class TKey, class TValue, Type::Integer TIndex = usize>
struct SimpleMap:
	Collected<TKey, TValue, KeyValuePair>,
	Derived<List<KeyValuePair<TKey, TValue>, TIndex>>,
	SelfIdentified<SimpleMap<TKey, TValue, TIndex>>,
	private List<KeyValuePair<TKey, TValue>, TIndex> {
public:
	static_assert(Type::Comparable::Threeway<TKey, TKey>, "Cannot form a map whithout an orderable key!");

	using Derived			= ::CTL::Derived<List<KeyValuePair<TKey, TValue>, TIndex>>;
	using Collected			= ::CTL::Collected<TKey, TValue, KeyValuePair>;
	using SelfIdentified	= ::CTL::SelfIdentified<SimpleMap<TKey, TValue, TIndex>>;

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
		typename BaseType::OrderType,
		typename BaseType::Order
	;

	using
		typename BaseType::PredicateType,
		typename BaseType::CompareType
	;

	using
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
		BaseType::filtered,
		BaseType::capacity,
		BaseType::empty
	;

	constexpr SimpleMap(): BaseType() {}

	constexpr SimpleMap(SizeType const& size): BaseType(size) {}

	constexpr SimpleMap(ArgumentListType const& values): BaseType(values) {
		filter(UNIQUE_VALUES).sort();
	}

	template<SizeType N>
	constexpr SimpleMap(Decay::AsType<PairType[N]> const& values): BaseType(values, N) {
		filter(UNIQUE_VALUES).sort();
	}

	constexpr SimpleMap(BaseType const& other): BaseType(other) {
		filter(UNIQUE_VALUES).sort();
	}

	constexpr SimpleMap(BaseType&& other): BaseType(CTL::move(other)) {
		filter(UNIQUE_VALUES).sort();
	}

	constexpr SimpleMap(SelfType const& other): BaseType(other) {}

	constexpr SimpleMap(SelfType&& other): BaseType(CTL::move(other)) {}

	constexpr SimpleMap(IteratorType const& begin, IteratorType const& end): BaseType(begin, end) {
		filter(UNIQUE_VALUES).sort();
	}

	constexpr SimpleMap(ReverseIteratorType const& begin, ReverseIteratorType const& end): BaseType(begin, end) {
		filter(UNIQUE_VALUES).sort();
	}

	constexpr ValueType at(KeyType const& key) const
	requires (Type::Constructible<ValueType>) {
		if (empty()) return pushBack({key}).back().value;
		IndexType i = find(key);
		if (i == -1)	return ValueType();
		else			return data()[i].value;
	}

	constexpr ValueType at(KeyType const& key) const
	requires (!Type::Constructible<ValueType>) {
		if (empty()) return pushBack({key}).back().value;
		IndexType i = find(key);
		if (i == -1)	throw OutOfBoundsException("Key does not exist!");
		else			return data()[i].value;
	}

	constexpr ValueType& operator[](KeyType const& key) {
		if (empty()) return pushBack({key}).back().value;
		IndexType i = find(key);
		if (i == -1) {
			BaseType::pushBack({key}).sort();
			return data()[find(key)].value;
		} else return data()[i].value;
	}

	constexpr IndexType find(KeyType const& key) const {
		if (empty()) return -1;
		IndexType pivot = size() / 2, index = pivot;
		while (pivot != 0) {
			switch (data()[index].key <=> key) {
				case Order::EQUAL:		return index;
				case Order::GREATER:	index -= (pivot /= 2);	break;
				case Order::LESS:		index += (pivot /= 2);	break;
				case Order::UNORDERED:	return -1;
			}
		}
		return -1;
	}

	constexpr ValueType operator[](KeyType const& index) const {return at(index);}

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

	constexpr SelfType& removeIf(PredicateType const& predicate) {
		BaseType::removeIf(predicate);
		return *this;
	}

	constexpr SelfType& removeIfNot(PredicateType const& predicate) {
		BaseType::removeIfNot(predicate);
		return *this;
	}

	constexpr SelfType& erase(KeyType const& key) {
		IndexType i = find(key);
		if (i == -1) return *this;
		BaseType::erase(i);
		return *this;
	}

	constexpr SelfType& eraseIf(PredicateType const& predicate) {
		BaseType::eraseIf(predicate);
		return *this;
	}

	constexpr SelfType& eraseIfNot(PredicateType const& predicate) {
		BaseType::eraseIfNot(predicate);
		return *this;
	}

	constexpr SelfType& insert(PairType const& pair) {
		if (!contains(pair.key))
			BaseType::pushBack(pair).sort();
		return *this;
	}

	constexpr SelfType& append(SelfType const& other) {
		BaseType::appendBack(other.filtered(NOT_IN_MAP)).sort();
		return *this;
	}

	constexpr SelfType& append(ArgumentListType const& values) {
		BaseType::appendBack(SelfType(values).filtered(NOT_IN_MAP)).sort();
		return *this;
	}

	constexpr SelfType& append(SizeType const& count, PairType const& fill) {
		BaseType::appendBack(SelfType(count, fill).filtered(NOT_IN_MAP)).sort();
		return *this;
	}

	constexpr SelfType& append(IteratorType const& begin, IteratorType const& end) {
		BaseType::appendBack(SelfType(begin, end).filtered(NOT_IN_MAP)).sort();
		return *this;
	}

	constexpr SelfType& append(ReverseIteratorType const& begin, ReverseIteratorType const& end) {
		BaseType::appendBack(SelfType(begin, end).filtered(NOT_IN_MAP)).sort();
		return *this;
	}

	template<SizeType S>
	constexpr SelfType& append(Decay::AsType<PairType[S]> const& values) {
		BaseType::appendBack(SelfType(values, S).filtered(NOT_IN_MAP)).sort();
		return *this;
	}

private:
	constexpr static CompareType const	UNIQUE_VALUES	= [](PairType const& a, PairType const& b){return a.key != b.key;};
	PredicateType const					NOT_IN_MAP		= [this](PairType const& pair){return !contains(pair.key);};
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
