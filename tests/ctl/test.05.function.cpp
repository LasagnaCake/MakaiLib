#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

using CTL::Function, CTL::String;

using namespace CTL::Literals::Text;

bool isPowerOf2(int x) {
    return x > 0 && !(x & (x-1));
}

void testFunction() {
	Function<void(void)>	f1	= []				{DEBUGLN("f1: Passed!");	};
	Function<int(void)>		f2	= []				{return 4;					};
	Function<int(int)>		f3	= [] (int n)		{return n * n;				};
	Function<int(int, int)>	f4	= [] (int a, int b)	{return a * b;				};
	f1();
	DEBUGLN("f2: ", f2());
	for(usize i = 1; i <= 10; ++i)
		DEBUGLN("f3: ", i, " -> ", f3(i));
	DEBUGLN("f4 {");
	DEBUGLN("+---+---", "--------"s * 10, "+");
	DEBUG("| x |");
	for(usize i = 2; i <= 11; ++i)
		DEBUG("\t", i);
	DEBUGLN("\t|");
	DEBUGLN("+---+---", "--------"s * 10, "+");
	for(usize i = 2; i <= 16; ++i) {
		DEBUG("|", i, " "s * ((i<10) ? 1 : 0), " |");
		for(usize j = 2; j <= 11; ++j) {
			if (isPowerOf2(f4(i, j)))
				DEBUG("\033[7;5m");
			DEBUG("\t", f4(i, j));
			DEBUG("\033[0m");
		}
		DEBUGLN("\033[0m\t|");
	}
	DEBUGLN("+---+---", "--------"s * 10, "+");
	DEBUGLN("}");
}

int main() {
	testFunction();
	DEBUGLN("Function test passed!");
	return 0;
}
