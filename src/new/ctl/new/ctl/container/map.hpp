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

#define NOT_IN_MAP [this](PairType const& p) -> bool {return notInMap(p);}

template<class TKey, class TValue, template <class TPairKey, class TPairValue> class TPair = Pair>
struct Collected {
	static_assert(Type::Pair<TPair<TKey, TValue>>, "Type is not a valid pair type!");

	typedef Typed<TKey>			Key;
	typedef Typed<TValue>		Value;

	typedef TKey				KeyType;
	typedef TValue				ValueType;
	typedef TPair<TKey, TValue>	PairType;
};

template<class TKey, class TValue, Type::Integer TIndex = usize, bool SORT = true>
struct BaseSimpleMap:
	Collected<TKey, TValue, KeyValuePair>,
	Derived<List<KeyValuePair<TKey, TValue>, TIndex>>,
	SelfIdentified<BaseSimpleMap<TKey, TValue, TIndex, SORT>>,
	private List<KeyValuePair<TKey, TValue>, TIndex> {
public:
	constexpr static bool SORTED = SORT;

	using Derived			= ::CTL::Derived<List<KeyValuePair<TKey, TValue>, TIndex>>;
	using Collected			= ::CTL::Collected<TKey, TValue, KeyValuePair>;
	using SelfIdentified	= ::CTL::SelfIdentified<BaseSimpleMap<TKey, TValue, TIndex, SORTED>>;

	using typename Derived::BaseType;

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

	static_assert(!SORTED || Sortable<KeyType>, "Cannot form a sortable map whithout an sortable key!");
	static_assert(!SORTED || Sortable<PairType>, "Cannot form a sortable map whithout an sortable pair type!");

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
		BaseType::front,
		BaseType::back,
		BaseType::begin,
		BaseType::end,
		BaseType::cbegin,
		BaseType::cend,
		BaseType::rbegin,
		BaseType::rend,
		BaseType::sort,
		BaseType::size,
		BaseType::filter,
		BaseType::filtered,
		BaseType::capacity,
		BaseType::empty
	;

	constexpr BaseSimpleMap(): BaseType() {}

	constexpr BaseSimpleMap(SizeType const& size): BaseType(size) {}

	constexpr BaseSimpleMap(ArgumentListType const& values): BaseType(values) {
		clean();
		update();
	}

	template<SizeType N>
	constexpr explicit BaseSimpleMap(Decay::AsType<PairType[N]> const& values): BaseType(values) {
		clean();
		update();
	}

	template<typename... Args>
	constexpr BaseSimpleMap(Args const&... args)
	requires (... && Type::Convertible<Args, PairType>)
	: BaseType(args...) {
		clean();
		update();
	}

	constexpr BaseSimpleMap(BaseType const& other): BaseType(other) {
		clean();
		update();
	}

	constexpr BaseSimpleMap(BaseType&& other): BaseType(CTL::move(other)) {
		clean();
		update();
	}

	constexpr BaseSimpleMap(SelfType const& other): BaseType(other) {}

	constexpr BaseSimpleMap(SelfType&& other): BaseType(CTL::move(other)) {}

	constexpr BaseSimpleMap(IteratorType const& begin, IteratorType const& end): BaseType(begin, end) {
		clean();
		update();
	}

	constexpr BaseSimpleMap(ReverseIteratorType const& begin, ReverseIteratorType const& end): BaseType(begin, end) {
		clean();
		update();
	}

	constexpr ValueType at(KeyType const& key) const
	requires (Type::Constructible<ValueType>) {
		if (empty()) return ValueType();
		IndexType i = search(key);
		if (i == -1)	return ValueType();
		else			return (data() + i)->value;
	}

	constexpr ValueType at(KeyType const& key) const
	requires (!Type::Constructible<ValueType>) {
		if (empty()) throw OutOfBoundsException("Key does not exist!");
		IndexType i = search(key);
		if (i == -1)	throw OutOfBoundsException("Key does not exist!");
		else			return (data() + i)->value;
	}

	constexpr ValueType& operator[](KeyType const& key) {
		if (empty()) return BaseType::pushBack(PairType(key)).back().value;
		IndexType i = search(key);
		if (i == -1) {
			BaseType::pushBack(PairType(key)).sort();
			return (data() + search(key))->value;
		} else return (data() + i)->value;
	}

	constexpr IndexType search(KeyType const& key) const
	requires (SORTED) {
		if (empty()) return -1;
		if (OrderType(front().key <=> key) == Order::EQUAL) return 0;
		if (OrderType(back().key <=> key) == Order::EQUAL) return size() - 1;
		IndexType lo = 0, hi = size() - 1, i = -1;
		SizeType loop = 0;
		while (hi >= lo & loop < size()) {
			i = lo + (hi - lo) / 2;
			switch(OrderType(key <=> (cbegin() + i)->key)) {
				case Order::LESS:		hi = i-1; break;
				case Order::EQUAL:		return i;
				case Order::GREATER:	lo = i+1; break;
				default:
				case Order::UNORDERED:	return -1;
			}
		}
		return -1;
	}

	constexpr IndexType search(KeyType const& key) const
	requires (!SORTED) {
		usize i = 0;
		for (auto& e: *this) {
			if (e.key == key)
				return i;
			++i;
		}
		return -1;
	}

	constexpr List<KeyType, SizeType> keys() const {
		List<KeyType, SizeType> result;
		for (auto& i: *this)
			result.pushBack(i.key);
		return result;
	}

	constexpr List<ValueType, SizeType> values() const {
		List<ValueType, SizeType> result;
		for (auto& i: *this)
			result.pushBack(i.value);
		return result;
	}

	constexpr List<PairType, SizeType> items() const {
		List<PairType, SizeType> result;
		for (auto& i: *this)
			result.pushBack(i);
		return result;
	}

	constexpr ValueType operator[](KeyType const& index) const {return at(index);}

	constexpr SelfType& operator=(SelfType const& other)	{BaseType::operator=(other); return *this;				}
	constexpr SelfType& operator=(SelfType&& other)			{BaseType::operator=(CTL::move(other)); return *this;	}

	constexpr bool contains(KeyType const& key) const {
		if (empty()) return false;
		return search(key) != -1;
	}

	constexpr SelfType& remove(KeyType const& key) {
		IndexType i = find(key);
		if (i == -1) return *	this;
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
		IndexType i = search(key);
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
		BaseType::appendBack(other);
		clean();
		update();
		return *this;
	}

	constexpr SelfType& append(ArgumentListType const& values) {
		return append(SelfType(values));
	}

	constexpr SelfType& append(SizeType const& count, PairType const& fill) {
		return append(SelfType(count, fill));
	}

	constexpr SelfType& append(IteratorType const& begin, IteratorType const& end) {
		return append(SelfType(begin, end));
	}

	constexpr SelfType& append(ReverseIteratorType const& begin, ReverseIteratorType const& end) {
		return append(SelfType(begin, end));
	}

	template<SizeType S>
	constexpr SelfType& append(Decay::AsType<PairType[S]> const& values) {
		return append(SelfType(values));
	}

private:
	/*
	constexpr static CompareType UNIQUE_VALUES(PairType const& a, PairType const& b) {
		return a.key != b.key;
	}
	//*/

	//*
	constexpr static auto const	UNIQUE_VALUES	= [](PairType const& a, PairType const& b){return a.key != b.key;};
	
	constexpr bool notInMap(PairType const& pair) const {
		return !contains(pair.key);
	}
	//*/

	constexpr void update() requires (SORTED)	{sort();	}
	constexpr void update() requires (!SORTED)	{			}

	constexpr void clean() {filter(UNIQUE_VALUES);}
};

#undef NOT_IN_MAP

/*
template<class TKey, class TValue, Type::Integer TIndex = usize, class TPair = Pair<TKey, TValue>, class THasher = Hasher>
requires (
	Type::Comparator::Threeway<TKey, TKey>
&&	PairType<TPair>
&&	Hashable<TKey, THasher>
) struct HashMap:
	List<TPair<TKey, TValue>, TIndex>,
	Collected<TKey, TValue, TPair>,
	SelfIdentified<HashMap<TKey, TValue, TIndex, TPair>> {

}
*/

template<class TKey, class TValue, Type::Integer TIndex = usize>
using OrderedMap	= BaseSimpleMap<TKey, TValue, TIndex, false>;

template<class TKey, class TValue, Type::Integer TIndex = usize>
using SimpleMap		= BaseSimpleMap<TKey, TValue, TIndex, true>;

template<class TKey, class TValue, Type::Integer TIndex = usize>
using Map	= SimpleMap<TKey, TValue, TIndex>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_MAP_H
