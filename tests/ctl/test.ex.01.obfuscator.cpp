#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using Obfuscator	= CTL::Ex::ObfuscatedStaticString<89>;
using Mangler		= CTL::Ex::MangledStaticString<64>;

Obfuscator	str		= Obfuscator("Flowering Nights");
Mangler		strm	= Mangler("Flowering Nights");

const CTL::String dostr = "Flowering Nights";

int main() {
	DEBUGLN("String: ", dostr);
	DEBUGLN("Mangled: ", strm.mangled());
	DEBUGLN("Demangled: ", strm.demangled());
	DEBUGLN("Deobfuscated: ", str.deobfuscated());
	DEBUGLN("Obfuscator test passed!");
	return 0;
}