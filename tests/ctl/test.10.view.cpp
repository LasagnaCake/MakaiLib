#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::View;

template<class T>
View<T> acquire() {
	static T val = 0;
	return View<T>(val);
}

template<class T>
void f1(View<T> v) {
	v = 9;
}

void testView() {
	View<int> view = acquire<int>();	DEBUGLN(view.value());
	view = 5;							DEBUGLN(view.value());
	f1(view);							DEBUGLN(view.value());
}

int main() {
	testView();
	return 0;
}
