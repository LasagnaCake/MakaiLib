#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::Map;

template<typename K, typename V>
void print(Map<K, V> const& mp) {
	DEBUG(
		"S: ",
		mp.size(),
		", C: ",
		mp.capacity(),
		", K: [ "
	);
	for (auto& i: mp) {
		DEBUG("[", i.key, " ", i.value, "] ");
	}
	DEBUGLN("];");
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
	CTL::Console::println("K", -2, ", L: ", mp.search(-2), ", V: ", mp[-2]);
	CTL::Console::println("K", -9, ", L: ", mp.search(-9), ", V: ", mp[-9]);
	CTL::Console::println("K", -4, ", L: ", mp.search(-4), ", V: ", mp[-4]);
	CTL::Console::println("K", 2, ", L: ", mp.search(2), ", V: ", mp[2]);
	print(mp);
	mp.append({{75, 25}, {88, 98}, {29, 0}, {0, -255}, {33, 33}});
	print(mp);
	DEBUGLN("Map test passed!");
	return 0;
}
