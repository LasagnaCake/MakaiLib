#include <makai.hpp>

struct TestApp: Makai::App {
	TestApp(): Makai::App(600, 400, "Test 03", false) {
		DEBUGLN("Shader loading BEGIN");
		try {
			loadShaders(
				Makai::SLF::loadFile("shaders/base/base.slf"),
				Makai::SLF::loadFile("shaders/framebuffer/compose.slf")
			);
		} catch (std::exception const& e) {
			Makai::Popup::showError(e.what());
			close();
		}
	}
};

int main() {
	try {
		TestApp app;
		app.run();
	} catch (std::exception const& e) {
		Makai::Popup::showError(e.what());
	}
	return 0;
}
