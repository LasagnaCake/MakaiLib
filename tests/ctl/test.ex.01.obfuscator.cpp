#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using Obfuscator	= CTL::Ex::ObfuscatedStaticString<89>;
using Mangler		= CTL::Ex::MangledStaticString<64>;

Obfuscator	str		= Obfuscator("Flowering Nights");
Mangler		strm	= Mangler("Flowering Nights");

const CTL::String dostr = "Flowering Nights";

int main() {
	DEBUGLN("String: [", dostr, "]");
	DEBUGLN("Mangled: [", strm.mangled().replace('\0', '*'), "]");
	DEBUGLN("Demangled: [", strm.demangled().replace('\0', '*'), "]");
	DEBUGLN("Deobfuscated: [", str.deobfuscated().replace('\0', '*'), "], size: ", str.deobfuscated().size());
	DEBUGLN("Obfuscator test passed!");
	return 0;
}