#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

namespace Err {
	void testError() {
		throw CTL::Error::NotAnError("Testing errors!", CTL_CPP_PRETTY_SOURCE);
	}

	struct FooTest {
		FooTest(int, bool t = false) {
			throw CTL::Error::NotAnError("Error inside constructor!", CTL_CPP_PRETTY_SOURCE);
		}
		
		static FooTest thing() {
			throw CTL::Error::NotAnError("Error inside static function!", CTL_CPP_PRETTY_SOURCE);
			return FooTest(0, 1);
		}
	};
}

void testError2() {
	throw CTL::Error::NotAnError("Another error test!");
}

void testError3() {
	Err::FooTest::thing();
}

void testError4() {
	Err::FooTest(0);
}

void printSource(CTL::CPP::SourceFile const& f) {
	DEBUGLN("Function: ", f.function);
	DEBUGLN("File: ", f.file);
	DEBUGLN("Line: ", f.line);
}

int main() {
	CTL::CPP::SourceFile file = CTL_CPP_PRETTY_SOURCE;
	printSource(file);
	try {Err::testError();} 
	catch (CTL::Error::Generic const& e) {DEBUGLN(e.report());}
	try {testError2();}
	catch (CTL::Error::Generic const& e) {DEBUGLN(e.report());}
	try {testError3();}
	catch (CTL::Error::Generic const& e) {DEBUGLN(e.report());}
	try {testError4();}
	catch (CTL::Error::Generic const& e) {DEBUGLN(e.report());}
	DEBUGLN("Error test passed!");
	return 0;
}