#include <makai/makai.hpp>

struct TestApp: Makai::App {
	Makai::Graph::Renderable cube;

	Makai::Graph::Camera3D& camera = Makai::Graph::Global::camera;

	Makai::Graph::Vertex const vertices[8] = {
		//					  X,  Y,  Z, U, V, R, G, B, A, NX, NY, NZ
		Makai::Graph::Vertex(+1, +1, +1, 0, 0, 1, 1, 1, 1, +1, +1, +1),	// 0
		Makai::Graph::Vertex(+1, +1, -1, 0, 0, 1, 1, 0, 1, +1, +1, -1),	// 1
		Makai::Graph::Vertex(+1, -1, +1, 0, 0, 1, 0, 1, 1, +1, -1, +1),	// 2
		Makai::Graph::Vertex(+1, -1, -1, 0, 0, 1, 0, 0, 1, +1, -1, -1),	// 3
		Makai::Graph::Vertex(-1, +1, +1, 0, 0, 0, 1, 1, 1, -1, +1, +1),	// 4
		Makai::Graph::Vertex(-1, +1, -1, 0, 0, 0, 1, 0, 1, -1, +1, -1),	// 5
		Makai::Graph::Vertex(-1, -1, +1, 0, 0, 0, 0, 1, 1, -1, -1, +1),	// 6
		Makai::Graph::Vertex(-1, -1, -1, 0, 0, 0, 0, 0, 1, -1, -1, -1)	// 7
	};

	virtual ~TestApp() {}

	TestApp(): Makai::App(600, 400, "Test 03", false) {
		DEBUGLN("Loading shaders...");
		loadDefaultShaders();
		DEBUGLN("Creating cube...");
		cube.material.culling = Makai::Graph::CullMode::OCM_FRONT;
		//cube.material.shaded = true;
		cube.trans.scale = .5;
		cube.triangles = {
			// Face +X
			new Makai::Graph::Triangle{{vertices[2], vertices[1], vertices[0]}},
			new Makai::Graph::Triangle{{vertices[1], vertices[2], vertices[3]}},
			// Face -X
			new Makai::Graph::Triangle{{vertices[4], vertices[5], vertices[6]}},
			new Makai::Graph::Triangle{{vertices[7], vertices[6], vertices[5]}},
			// Face +Y
			new Makai::Graph::Triangle{{vertices[0], vertices[1], vertices[4]}},
			new Makai::Graph::Triangle{{vertices[5], vertices[4], vertices[1]}},
			// Face -Y
			new Makai::Graph::Triangle{{vertices[6], vertices[3], vertices[2]}},
			new Makai::Graph::Triangle{{vertices[3], vertices[6], vertices[7]}},
			// Face +Z
			new Makai::Graph::Triangle{{vertices[4], vertices[2], vertices[0]}},
			new Makai::Graph::Triangle{{vertices[2], vertices[4], vertices[6]}},
			// Face -Z
			new Makai::Graph::Triangle{{vertices[1], vertices[3], vertices[5]}},
			new Makai::Graph::Triangle{{vertices[7], vertices[5], vertices[3]}}
		};
		DEBUGLN("Done!");
	}

	void onOpen() override {
		camera.eye	= Vec3(0, 2, -3);
		camera.at	= Vec3(0, 0, 0);
		camera.zFar	= 1000;
		cube.setRenderLayer(0);
		//Makai::Graph::API::toggle(Makai::Graph::API::Facility::GAF_DEBUG, true);
	}

	void onLogicFrame(float delta) {
		if (input.isButtonJustPressed(Makai::Input::KeyCode::KC_ESCAPE))
			close();
		getFrameBuffer().material.background = Makai::Graph::Color::WHITE * (sin(getCurrentFrame() / 180.0) / 2 + .5);
		#ifndef MANUAL_ROTATION
		cube.trans.rotation += Vec3(
			HPI / 60.0,
			HPI / 90.0/*,
			HPI / 45.0*/
		) / 2.0;
		#else
		cube.trans.rotation.x += (
			input.isButtonDown(Makai::Input::KeyCode::KC_UP)
			- input.isButtonDown(Makai::Input::KeyCode::KC_DOWN)
		) / 60.0;
		cube.trans.rotation.y += (
			input.isButtonDown(Makai::Input::KeyCode::KC_LEFT)
			- input.isButtonDown(Makai::Input::KeyCode::KC_RIGHT)
		) / 60.0;
		#endif // MANUAL_ROTATION
	}
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
