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
	TestApp app;
	try {
		app.run();
	} catch (Error::Error const& e) {
		Makai::Popup::showError(e.what());
	}
	return 0;
}
