#include <makai/makai.hpp>

struct TestApp: Makai::App {
	TestApp(): Makai::App(600, 400, "Test 02", false) {}
};

int main() {
	try {
		TestApp app;
		app.run();
	} catch (Error::Error const& e) {
		Makai::Popup::showError(e.what());
	} catch (std::runtime_error const& e) {
		Makai::Popup::showError(e.what());
	}
	return 0;
}
