#include <makai.hpp>

struct TestApp: Makai::App {
	Makai::Graph::Renderable cube;

	Makai::Graph::Vertex vertices[8] = {
		Makai::Graph::Vertex(+1, +1, +1, 0, 0, 1, 1, 1, 1, +1, +1, +1),	// 0
		Makai::Graph::Vertex(+1, +1, -1, 0, 0, 1, 1, 1, 1, +1, +1, -1),	// 1
		Makai::Graph::Vertex(+1, -1, +1, 0, 0, 1, 1, 1, 1, +1, -1, +1),	// 2
		Makai::Graph::Vertex(+1, -1, -1, 0, 0, 1, 1, 1, 1, +1, -1, -1),	// 3
		Makai::Graph::Vertex(-1, +1, +1, 0, 0, 1, 1, 1, 1, -1, +1, +1),	// 4
		Makai::Graph::Vertex(-1, +1, -1, 0, 0, 1, 1, 1, 1, -1, +1, -1),	// 5
		Makai::Graph::Vertex(-1, -1, +1, 0, 0, 1, 1, 1, 1, -1, -1, +1),	// 6
		Makai::Graph::Vertex(-1, -1, -1, 0, 0, 1, 1, 1, 1, -1, -1, -1)	// 7
	};

	virtual ~TestApp() {}

	TestApp(): Makai::App(600, 400, "Test 03", false) {
		DEBUGLN("Shader loading BEGIN");
		loadShaders(
			Makai::SLF::loadFile("shaders/base/base.slf"),
			Makai::SLF::loadFile("shaders/framebuffer/compose.slf")
		);
		Makai::Graph::Camera3D& cam = Makai::Graph::Global::camera;
		cam.eye		= Vec3(0, 0, -1);
		cam.at		= Vec3(0, 0, 0);
		cam.zFar	= 1000;
		cam.relativeToEye = true;
	}

	void onOpen() override {
		cube.setRenderLayer(0);
		cube.material.shaded = true;
		cube.triangles = {
			// Face +X
			new Makai::Graph::Triangle{{vertices[0], vertices[1], vertices[2]}},
			new Makai::Graph::Triangle{{vertices[1], vertices[2], vertices[3]}},
			// Face -X
			new Makai::Graph::Triangle{{vertices[4], vertices[5], vertices[6]}},
			new Makai::Graph::Triangle{{vertices[5], vertices[6], vertices[7]}},
			// Face +Y
			new Makai::Graph::Triangle{{vertices[0], vertices[1], vertices[4]}},
			new Makai::Graph::Triangle{{vertices[1], vertices[4], vertices[5]}},
			// Face -Y
			new Makai::Graph::Triangle{{vertices[2], vertices[3], vertices[6]}},
			new Makai::Graph::Triangle{{vertices[3], vertices[6], vertices[7]}},
			// Face +Z
			new Makai::Graph::Triangle{{vertices[0], vertices[2], vertices[4]}},
			new Makai::Graph::Triangle{{vertices[2], vertices[4], vertices[6]}},
			// Face -Z
			new Makai::Graph::Triangle{{vertices[1], vertices[3], vertices[5]}},
			new Makai::Graph::Triangle{{vertices[3], vertices[5], vertices[7]}},
		};
	}

	void onLogicFrame(float delta) {
		getFrameBuffer().material.background = Makai::Graph::Color::PURPLE * (sin(getCurrentFrame() / 180.0) / 2 + .5);
		Makai::Graph::Camera3D& cam = Makai::Graph::Global::camera;
		if (input.isButtonDown(Makai::Input::KeyCode::KC_UP))		cam.eye.z += 1 / 60.0;
		if (input.isButtonDown(Makai::Input::KeyCode::KC_DOWN))		cam.eye.z -= 1 / 60.0;
		if (input.isButtonDown(Makai::Input::KeyCode::KC_LEFT))		cam.eye.x += 1 / 60.0;
		if (input.isButtonDown(Makai::Input::KeyCode::KC_RIGHT))	cam.eye.x -= 1 / 60.0;
		cube.trans.rotation += Vec3(
			HPI / 60.0,
			HPI / 90.0,
			HPI / 120.0
		);
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
