#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

void testPseudoRandom() {
	DEBUGLN("Testing pseudo-random...");
	CTL::Random::SimpleGenerator rng;
	for (usize i = 0; i < 20; ++i) DEBUGLN(rng.number<usize>());
	for (usize i = 0; i < 20; ++i) DEBUGLN(rng.number<float>());
	DEBUGLN("");
	for (usize i = 0; i < 10; ++i) DEBUGLN((int)rng.number<int>(0, 100));
	for (usize i = 0; i < 10; ++i) DEBUGLN(rng.number<float>(0, 100));
}

void testSecureRandom() {
	DEBUGLN("Testing secure-random...");
	CTL::Random::SecureGenerator rng;
	for (usize i = 0; i < 20; ++i) DEBUGLN(rng.number<usize>());
	for (usize i = 0; i < 20; ++i) DEBUGLN(rng.number<float>());
	DEBUGLN("");
	for (usize i = 0; i < 10; ++i) DEBUGLN(rng.number<int>(0, 10));
	for (usize i = 0; i < 10; ++i) DEBUGLN(rng.number<float>(0, 10));
}

int main() {
	testPseudoRandom();
	testSecureRandom();
	DEBUGLN("RNG test passed!");
	return 0;
}
