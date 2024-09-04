#include <makai.hpp>

struct TestApp: Makai::App {
	TestApp(): Makai::App(600, 400, "Test 02", false) {}
};

int main() {
	TestApp app;
	app.run();
	return 0;
}
