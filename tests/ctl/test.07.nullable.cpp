#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::Nullable, CTL::String;

template<class T>
void print(Nullable<T> const& p) {
	if (p)	DEBUGLN(p.value());
	else	DEBUGLN("Oops!");
}

int main() {
	Nullable<String> maybe;		print(maybe);
	maybe = "Quantifiable?";	print(maybe);
	maybe = nullptr;			print(maybe);
	return 0;
}
