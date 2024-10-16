#ifndef CTL_EX_CONTAINER_GROUPS_H
#define CTL_EX_CONTAINER_GROUPS_H

#include "../../ctl/exnamespace.hpp"
#include "../../ctl/typetraits/traits.hpp"
#include "../../ctl/templates.hpp"

CTL_EX_NAMESPACE_BEGIN

template<class TData, class TIdentifier = usize, Type::Integer TIndex = usize>
struct Groups:
	::CTL::Collected<TIdentifier, TData, KeyValuePair>
	::CTL::Iteratable<KeyValuePair<TIdentifier, TData>, TIndex>,
	::CTL::SelfIdentified<Groups<TData, TIdentifier, TIndex>> {
public:
	static_assert(Type::Comparable::Threeway<TIdentifier, TIdentifier>, "Identifier must be comparable!");

	using Collected			= ::CTL::Collected<TIdentifier, TData>;
	using Iteratable		= ::CTL::Iteratable<KeyValuePair<TIdentifier, TData>, TIndex>;
	using SelfIdentified	= ::CTL::SelfIdentified<Groups<TData, TIdentifier, TIndex>>;

	using
		typename Collected::KeyType,
		typename Collected::ValueType,
		typename Collected::PairType
	;

	using
		typename Iteratable::SizeType
	;

	using typename SelfIdentified::SelfType;

	using GroupType		= List<ValueType, SizeType>;
	using IDListType	= List<KeyType, SizeType>;

	constexpr GroupType& get(KeyType const& id) {
		if (!g.contains(id))
			flush(id);
		return g[id];
	}

	constexpr GroupType& operator[](KeyType const& id) {
		return get(id);
	}

	constexpr GroupType& withObject(KeyType const& obj) {
		IDListType gs;
		try {
			for (auto const& group: g)
				if (group.value.find(obj))
					gs.pushBack(group.key);
		} catch(...) {}
		return gs;
	}

	constexpr SelfType& add(ValueType const& obj, KeyType const& groupID) {
		get(groupID).pushBack(obj);
		return *this;
	}

	constexpr SelfType& remove(ValueType const& obj, KeyType const& groupID) {
		GroupType& gp = get(groupID);
		gp.erase(gp.find(obj));
		return *this;
	}

	constexpr SelfType& removeFromAll(ValueType const& obj) {
		IDListType gs = withObject(obj);
		for (auto group: gs)
			remove(obj, group);
		return *this;
	}

	constexpr SelfType& flush(KeyType const& id) {
		g[id] = GroupType();
		return *this;
	}

	constexpr bool contains(ValueType const& obj, KeyType const& groupID) const {
		return g[groupID].find(obj) != -1;
	}

	constexpr IDListType all() const {
		return g.keys();
	}

	constexpr auto begin()			{return g.begin();	}
	constexpr auto end()			{return g.end();	}
	constexpr auto cbegin()			{return g.cbegin();	}
	constexpr auto cend()			{return g.cend();	}
	constexpr auto rbegin()			{return g.rbegin();	}
	constexpr auto rend()			{return g.rend();	}

	constexpr auto begin() const	{return g.begin();	}
	constexpr auto end() const		{return g.end();	}
	constexpr auto cbegin() const	{return g.cbegin();	}
	constexpr auto cend() const		{return g.cend();	}
	constexpr auto rbegin() const	{return g.rbegin();	}
	constexpr auto rend() const		{return g.rend();	}

private:
	Map<KeyType, GroupType> g;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_CONTAINER_GROUPS_H