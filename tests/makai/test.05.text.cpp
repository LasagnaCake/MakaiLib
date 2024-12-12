#include <makai/makai.hpp>

struct TestApp: Makai::App {
	TestApp(): Makai::App(600, 400, "Test 05", false) {
		DEBUGLN("Loading shaders...");
		loadDefaultShaders();
	}
	
	Makai::Graph::Label label;

	Makai::Graph::Camera3D& camera = Makai::Graph::Global::camera;

	virtual ~TestApp() {}

	void onOpen() override {
		DEBUGLN("1...");
		try {
			//label.font->image.create("../tests/makai/files/TestFontGrid-lotuscoder.png");
		} catch (Makai::Error::Generic const& e) {
			Makai::Popup::showError(e.what());
			close();
			return;
		}
		DEBUGLN("2...");
		label.trans.position	= Makai::Math::Vec3(0, 0, 0);
		label.text->textAlign	=
		label.text->rectAlign	= Makai::Math::Vec2(.5);
		DEBUGLN("3...");
		label.text->content		= "Sexyman Acidosis ~ Whomst'd've!!!\nNarcolepsy";
		label.text->rect		= {40, 2};
		DEBUGLN("4...");
		camera.eye	= Makai::Vec3(0, 0, -1);
		camera.zFar	= 1000;
		getFrameBuffer().material.background = Makai::Graph::Color::GRAY;
		//Makai::Graph::API::toggle(Makai::Graph::API::Facility::GAF_DEBUG, true);
		//camera.relativeToEye = true;
		DEBUGLN("Done!");
	}

	void onUpdate(float delta) {
		camera.eye	= Makai::Vec3(sin(getCurrentCycle()), 0, cos(getCurrentCycle()));
	}
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
