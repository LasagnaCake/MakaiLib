#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

int main() {
	DEBUGLN(CTL::Regex::replace("0123456789abcdef", "[0-3]|[6-9]|[c-e]", "($&)"));
	DEBUGLN(CTL::Regex::replace("0123456789abcdef", "([0-3]|[6-9]|[c-e])+", "{$&}"));
	DEBUGLN(CTL::Regex::replace("0123456789abcdef", "([0-9])", "/"));
	DEBUGLN(CTL::Regex::replace("0123456789abcdef", "([0-9]+)", "/$&/"));
	DEBUGLN("Regex test passed!");
	return 0;
}
