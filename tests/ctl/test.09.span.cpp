#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::Span;

template<typename T, usize S>
void print(Span<T, S> const& s) {
	DEBUG(
		"S: ",
		s.size(),
		", I: [ "
	);
	for (auto& i: s) {
		DEBUG(i, " ");
	}
	DEBUGLN("];");
}

void testSpan() {
	int arr[] = {1, 4, 7, 8, 4, 3, 2, 5, 9};
	print(Span<int, 5>(arr));
	print(Span<int, 3>(arr+4));
	print(Span<int>(arr + 1, arr + 7));
	print(Span<int>(arr + 2, 5));
}

int main() {
	testSpan();
	DEBUGLN("Span test passed!");
	return 0;
}
