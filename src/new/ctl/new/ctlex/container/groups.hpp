#ifndef CTL_EX_CONTAINER_GROUPS_H
#define CTL_EX_CONTAINER_GROUPS_H

#include "../../ctl/exnamespace.hpp"
#include "../../ctl/typetraits/traits.hpp"
#include "../../ctl/templates.hpp"

CTL_EX_NAMESPACE_BEGIN

template<class TData, class TIdentifier = usize, Type::Integer TIndex = usize>
struct Groups:
	Collected<TIdentifier, TData, KeyValuePair>,
	Iteratable<KeyValuePair<TIdentifier, TData>, TIndex>,
	SelfIdentified<Groups<TData, TIdentifier, TIndex>> {
public:
	static_assert(Type::Comparator::Threeway<TIdentifier, TIdentifier>, "Identifier must be comparable!");

	using Collected			= ::CTL::Collected<TIdentifier, TData>;
	using Iteratable		= ::CTL::Iteratable<KeyValuePair<TIdentifier, TData>, TIndex>;
	using SelfIdentified	= ::CTL::SelfIdentified<Groups<TData, TIdentifier, TIndex>>;

	using KeyType	= Collected::KeyType;
	using ValueType	= Collected::ValueType;
	using PairType	= Collected::PairType;

	using
		SizeType = Iteratable::SizeType
	;

	using GroupType				= List<ValueType, SizeType>;
	using IdentifierListType	= List<KeyType, SizeType>;

	using CollectionType = Map<KeyType, GroupType, SizeType>;

	using typename SelfIdentified::SelfType;

	constexpr GroupType& get(KeyType const& id) {
		if (!g.contains(id))
			flush(id);
		return g[id];
	}

	constexpr GroupType& operator[](KeyType const& id) {
		return get(id);
	}

	constexpr IdentifierListType withObject(ValueType const& obj) {
		if (group.empty()) return IdentifierListType();
		IdentifierListType gs;
		try {
			DEBUGLN("Finding groups with object...");
			for (auto const& group: g)
				if (group.value.find(obj) != -1)
					gs.pushBack(group.key);
			DEBUGLN("Success");
		} catch(...) {
			DEBUGLN("Error");
		}
		DEBUGLN("Found ", gs.size(), " instances");
		return gs;
	}

	constexpr SelfType& add(ValueType const& obj, KeyType const& groupID) {
		get(groupID).pushBack(obj);
		return *this;
	}

	constexpr SelfType& remove(ValueType const& obj, KeyType const& groupID) {
		GroupType& gp = get(groupID);
		gp.eraseLike(obj);
		return *this;
	}

	constexpr SelfType& removeFromAll(ValueType const& obj) {
		DEBUGLN("Getting object's groups...");
		IdentifierListType gs = withObject(obj);
		if (gs.empty()) return *this;
		DEBUGLN("Removing object from group...");
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

	constexpr IdentifierListType all() const {
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
	CollectionType g;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_CONTAINER_GROUPS_H