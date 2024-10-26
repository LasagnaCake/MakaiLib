#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

void testPseudoRandom() {
	DEBUGLN("Testing pseudo-random...");
	CTL::Random::PseudoGenerator rng;
	DEBUGLN(rng.number<usize>());
	DEBUGLN(rng.number<float>());
	DEBUGLN(rng.number<int>(0, 10));
	DEBUGLN(rng.number<float>(0, 10));
}

void testSecureRandom() {
	DEBUGLN("Testing secure-random...");
	CTL::Random::SecureGenerator rng;
	DEBUGLN(rng.number<usize>());
	DEBUGLN(rng.number<float>());
	DEBUGLN(rng.number<int>(0, 10));
	DEBUGLN(rng.number<float>(0, 10));
}

int main() {
	testPseudoRandom();
	testSecureRandom();
	DEBUGLN("RNG test passed!");
	return 0;
}
