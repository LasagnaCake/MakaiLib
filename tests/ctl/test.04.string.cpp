#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::String, CTL::List;

void print(String const& str) {
	DEBUGLN(
		"S: ",
		str.size(),
		", C: ",
		str.capacity(),
		", S: [ \"",
		str,
		"\" ];"
	);
}

void print(List<String> const& strs) {
	DEBUG(
		"S: ",
		strs.size(),
		", C: ",
		strs.capacity(),
		", L: [ "
	);
	for (auto& s : strs)
		DEBUG("\"", s ,"\" ");
	DEBUGLN(
		"];"
	);
}

void testString() {
	String str;							print(str);
	str = "Henlo.";						print(str);
	str += " You?";						print(str);
	str = "O! " + str;					print(str);
	print(str.sliced(2, -3));
	str.sort();							print(str);
	str = String("Impedance. Voltage. Current.");
	print(str);
	auto sp = str.splitAtFirst(' ');	print(sp);
	sp = str.splitAtLast(' ');			print(sp);
	sp = str.split(' ');				print(sp);
	// This causes a crash
	sp = sp.sort();						print(sp);
}

void testStringMap() {
}

int main() {
	testString();
	DEBUGLN("String test passed!");
	return 0;
}
