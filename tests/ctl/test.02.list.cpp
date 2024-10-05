#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::List;

template<typename T>
void printList(List<T> const& lst) {
	DEBUG("List items: [ ");
	for (auto& i: lst) {
		DEBUG(i, " ");
	}
	DEBUGLN("];");
}

int main() {
	List<int> lst({0, 1, 17, 5, 9, 8});			printList(lst);
	lst.pushBack(69);							printList(lst);
	lst.popBack();								printList(lst);
	lst.appendBack({4, 5, 8});					printList(lst);
	lst.eraseIf([](auto e){return e > 10;});	printList(lst);
	lst.insert({-5, 9, 9, 8, 9, -20}, 2);		printList(lst);
	lst.reverse();								printList(lst);
	lst.sort();									printList(lst);
	lst.clear();								printList(lst);
}
