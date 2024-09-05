#include <makai.hpp>

struct TestApp: Makai::App {
	TestApp(): Makai::App(600, 400, "Test 03", false) {
		DEBUGLN("Shader loading BEGIN");
		loadShaders(
			Makai::SLF::loadFile("shaders/base/base.slf"),
			Makai::SLF::loadFile("shaders/framebuffer/compose.slf")
		);
	}
};

int main() {
	try {
		TestApp app;
		app.run();
	} catch (std::runtime_error const& e) {
		Makai::Popup::showError(e.what());
	}
	return 0;
}
