#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::List;

template<typename T>
void printList(List<T> const& lst) {
	DEBUG(
		"S: ",
		lst.size(),
		", C: ",
		lst.capacity(),
		", I: [ "
	);
	for (auto& i: lst) {
		DEBUG(i, " ");
	}
	DEBUGLN("];");
}

int main() {
	List<int> lst({20, -64, -20, 20, 0, -1, 17, -5, 1, 28, 34, 6, 4, -1, 1, -4, 57, 8, -19, 19, 8});
	printList(lst);
	lst.pushBack(69);							printList(lst);
	lst.appendBack({4, 420, 8});				printList(lst);
	lst.popBack();								printList(lst);
	lst.eraseIf([](auto e){return e > 10;});	printList(lst);
	lst.reserve(48);							printList(lst);
	lst.insert({-5, 9, 9, 8, 9, -20}, 2);		printList(lst);
	lst = lst.uniques();						printList(lst);
	lst.reverse();								printList(lst);
	lst.sort();									printList(lst);
	lst.clear();								printList(lst);
	lst.dispose();								printList(lst);
	DEBUGLN("List test passed!");
}
