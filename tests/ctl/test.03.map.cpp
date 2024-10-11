#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using
	CTL::BaseSimpleMap,
	CTL::OrderedMap,
	CTL::SimpleMap,
	CTL::List,
	CTL::KeyValuePair,
	CTL::TypeInfo
;

template<typename T>
void print(List<T> const& lst) {
	DEBUG(
		"S: ",
		lst.size(),
		", C: ",
		lst.capacity(),
		", E: [ "
	);
	for (auto& i: lst) {
		DEBUG(i, " ");
	}
	DEBUGLN("];");
}

template<typename K, typename V, typename I, bool S>
void print(BaseSimpleMap<K, V, I, S> const& m) {
	DEBUG(
		"S: ",
		m.size(),
		", C: ",
		m.capacity(),
		", I: [ "
	);
	for (auto& i: m) {
		DEBUG("[", i.key, " ", i.value, "] ");
	}
	DEBUGLN("];");
}

template<typename K, typename V>
void print(List<KeyValuePair<K, V>> const& m) {
	DEBUG(
		"S: ",
		m.size(),
		", C: ",
		m.capacity(),
		", P: [ "
	);
	for (auto& i: m) {
		DEBUG("[", i.key, " ", i.value, "] ");
	}
	DEBUGLN("];");
}

template<typename K, typename V, typename I, bool S>
void print(BaseSimpleMap<K, V, I, S> const& m, K const& k) {
	auto r = m.search(k);
	if (r != -1)
		CTL::Console::println("K: ", k, ", L: ", r, ", V: ", m[k]);
	else
		CTL::Console::println("K: ", k, ", L: ", r);
}

template<template<typename K, typename V> class TMap = SimpleMap>
void testMap() {
	using MapType = TMap<int, int>;
	DEBUGLN("<", TypeInfo<MapType>::name(), ">");
	MapType mp({
		{0, 1},
		{1, 2},
		{4, 5},
		{3, 6},
		{12, 9},
		{14, 9},
		{15, 9},
		{16, 9},
		{-5, -4}
	});
	print(mp);
	mp = MapType({
		{2, -32},
		{29, -31},
		{-4, -30},
		{448, -29},
		{448, -28},
		{448, -27},
		{448, -26},
		{448, -25},
		{448, -24},
		{448, -23},
		{448, -22},
		{448, -21},
		{-9, -20},
		{-2, -19}
	});
	print(mp);
	mp[33] = 28;
	print(mp);
	print(mp, -2);
	print(mp, -9);
	print(mp, -29);
	print(mp, -4);
	print(mp, 448);
	mp.append({{75, 25}, {-9, 98}, {29, 0}, {0, -255}, {33, 33}});
	print(mp);
	print(mp.keys());
	print(mp.values());
	print(mp.items());
	mp.clear();		print(mp);
	mp.dispose();	print(mp);
	DEBUGLN("</", TypeInfo<MapType>::name(), ">");
}

int main() {
	testMap<SimpleMap>();
	testMap<OrderedMap>();
	DEBUGLN("Map test passed!");
	return 0;
}
