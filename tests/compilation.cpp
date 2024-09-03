#include <makai.hpp>

int main() {
	#ifdef _DEBUG_OUTPUT_
	DEBUGLN("Compilation test passed");
	#endif // _DEBUG_OUTPUT_
	Makai::Popup::showDialog("Passed", "Compilation test passed");
	return 0;
}
