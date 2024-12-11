#include <makai/makai.hpp>

struct TestApp: Makai::App {
	TestApp(): Makai::App(600, 400, "Test 05", false) {
		DEBUGLN("Loading shaders...");
		loadDefaultShaders();
	}
	
	Makai::Graph::Label label;

	virtual ~TestApp() {}

	void onOpen() override {
		DEBUGLN("1...");
		try {
			label.font->image.create("../tests/makai/files/TestFontGrid-lotuscoder.png");
		} catch (Makai::Error::Generic const& e) {
			Makai::Popup::showError(e.what());
			close();
			return;
		}
		DEBUGLN("2...");
		label.trans.position	= Makai::Math::Vec2(32, -32) * getWindowScale();
		label.text->textAlign	=
		label.text->rectAlign	= Makai::Math::Vec2(.5);
		DEBUGLN("3...");
		label.text->content		= "Sexyman Acidosis ~ Whomst'd've!!!";
		label.text->rect		= {40, 2};
		DEBUGLN("4!");
	}

	void onLogicFrame(float delta) {}
};

int main() {
	try {
		TestApp app;
		app.run();
	} catch (Makai::Error::Generic const& e) {
		Makai::Popup::showError(e.what());
	}
	return 0;
}
