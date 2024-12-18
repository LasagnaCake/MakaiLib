#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

void testError() {
	throw CTL::Error::NotAnError("Testing errors!");
}

void testError2() {
	testError();
}

void printSource(CTL::CPP::SourceFile const& f) {
	DEBUGLN("Function: ", f.function);
	DEBUGLN("File: ", f.file);
	DEBUGLN("Line: ", f.line);
}

int main() {
	try {
		CTL::CPP::SourceFile file = CTL_CPP_PRETTY_SOURCE;
		printSource(file);
		testError2();
	} catch (CTL::Error::Generic const& e) {
		DEBUGLN(e.report());
	}
	DEBUGLN("Error test passed!");
	return 0;
}