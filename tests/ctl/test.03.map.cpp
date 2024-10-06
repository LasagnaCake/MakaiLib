#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::Map;

template<typename K, typename V>
void printList(Map<K, V> const& mp) {
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
	Map<int, int> mp;
	printList(mp);
	mp = Map<int, int>({
		{0, 1},
		{1, 2},
		{4, 5},
		{3, 6},
		{12, 9},
		{-5, -4}
	});
	DEBUGLN("Map test passed!");
}
