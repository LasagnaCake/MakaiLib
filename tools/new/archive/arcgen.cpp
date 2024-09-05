#define ARCSYS_APPLICATION_

#include <archive.hpp>

String escape(char const& c) {
	switch (c) {
		case '\'':	return "\\'";
		case '\"':	return "\\\"";
		case '\?':	return "\\?";
		case '\\':	return "\\\\";
		case '\a':	return "\\a";
		case '\b':	return "\\b";
		case '\f':	return "\\f";
		case '\n':	return "\\n";
		case '\r':	return "\\r";
		case '\t':	return "\\t";
		case '\v':	return "\\v";
		default:	return toString(c);
	}
}

int main(int argc, char** argv) {
	DEBUGLN("Starting...");
	if (argc < 2)
		DEBUGLN(
			"\n\nHow to use ArcGen:\n\n"
			"arcgen.exe \"YOUR_PASSWORD_HERE\""
		);
	else if (argc >= 2) {
		String keyfile = "constexpr ObfuscatedString<32> const passkey = ObfuscatedString<32>(\"";
		for (char& c: Makai::Tool::Arch::hashPassword(argv[1])) {
			std::stringstream stream;
			stream << std::hex << (unsigned int)(unsigned char)(c);
			std::string code = stream.str();
			keyfile += std::string("\\x")+(code.size()<2?"0":"")+code;
		}
		keyfile += "\");";
		FileLoader::saveTextFile("key.256.h", keyfile);
		DEBUGLN("Key generated!");
	}
	return 0;
}
