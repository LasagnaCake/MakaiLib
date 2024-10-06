#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::Map;

template<typename K, typename V>
void print(Map<K, V> const& m) {
	DEBUG(
		"S: ",
		m.size(),
		", C: ",
		m.capacity(),
		", K: [ "
	);
	for (auto& i: m) {
		DEBUG("[", i.key, " ", i.value, "] ");
	}
	DEBUGLN("];");
}

template<typename K, typename V>
void print(Map<K, V> const& m, K const& k) {
	CTL::Console::println("K: ", k, ", L: ", m.search(k), ", V: ", m[k]);
}

int main() {
	Map<int, int> mp = Map<int, int>({
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
	mp = Map<int, int>({
		{2, -32},
		{29, -32},
		{-4, -32},
		{448, -32},
		{-9, -32},
		{-2, -32},
	});
	print(mp);
	mp[33] = 28;
	print(mp);
	print(mp, -2);
	print(mp, -9);
	print(mp, -29);
	print(mp, -4);
	print(mp, 448);
	mp.append({{75, 25}, {88, 98}, {29, 0}, {0, -255}, {33, 33}});
	print(mp);
	DEBUGLN("Map test passed!");
	return 0;
}
