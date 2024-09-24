#include <makai/makai.hpp>

int main() {
	#ifdef MAKAILIB_DEBUG
	DEBUGLN("Compilation test passed");
	#endif // MAKAILIB_DEBUG
	Makai::Popup::showDialog("Passed", "Compilation test passed");
	return 0;
}
