#include <makai.hpp>

struct TestApp: Makai::App {
	Makai::Graph::Renderable cube;

	VecMath::Transform3D planes[6] = {
		VecMath::Transform3D(Vec3(0, +1, 0),	Vec3(+HPI, 0, 0),	1),
		VecMath::Transform3D(Vec3(0, -1, 0),	Vec3(-HPI, 0, 0),	1),
		VecMath::Transform3D(Vec3(0, 0, +1),	Vec3(0, 0, 0),		1),
		VecMath::Transform3D(Vec3(0, 0, -1),	Vec3(-PI, 0, 0),	1),
		VecMath::Transform3D(Vec3(+1, 0, 0),	Vec3(0, +HPI, 0),	1),
		VecMath::Transform3D(Vec3(-1, 0, 0),	Vec3(0, -HPI, 0),	1),
	};

	virtual ~TestApp() {}

	TestApp(): Makai::App(600, 400, "Test 03", false) {
		DEBUGLN("Shader loading BEGIN");
		loadShaders(
			Makai::SLF::loadFile("shaders/base/base.slf"),
			Makai::SLF::loadFile("shaders/framebuffer/compose.slf")
		);
		Makai::Graph::Camera3D& cam = Makai::Graph::Global::camera;
		cam.eye		= Vec3(0, 0, -10);
		cam.at		= Vec3(0, 0, 0);
		cam.zFar	= 1000;
		cube.setRenderLayer(1);
	}

	void onOpen() override {
		for(usize i = 0; i < 6; ++i) {
			Makai::Graph::PlaneRef* ref = cube.createReference<Makai::Graph::PlaneRef>();
			ref->local = planes[i];
		}
		cube.material.shaded = true;
		getFrameBuffer().material.background = Makai::Graph::Color::PURPLE;
	}

	void onLogicFrame(float delta) {
		/*cube.trans.rotation += Vec3(
			HPI / 60.0,
			HPI / 90.0,
			HPI / 120.0
		);*/
	}
};

int main() {
	try {
		TestApp app;
		app.run();
	} catch (Error::Error const& e) {
		Makai::Popup::showError(e.what());
	}
	return 0;
}
