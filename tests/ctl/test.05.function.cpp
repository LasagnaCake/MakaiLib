#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::Function;

void testFunction() {
	Function<void(void)> f1 = []{DEBUGLN("f1: Passed!");};
}

int main() {
	testFunction();
	return 0;
}
