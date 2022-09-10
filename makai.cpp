// #include <irrlicht.h>
#include <stdexcept>

#include "src/program.hpp"

#include <windows.h>

//#define SIDE_B

class GameApp: public Makai::Program{
public:
	GameApp(
		unsigned int width,
		unsigned int height,
		std::string windowTitle,
		bool fullscreen = false
	) : Program (
			width,
			height,
			windowTitle,
			fullscreen,
			"shaders/framebuffer/compose.slf"
	) {};
	RenderData::Renderable testRenderable;
	RenderData::Renderable testRenderable2;
	Drawer::Texture2D ringu;

	void onOpen() override {
		ringu.create("ring.png");
		getFrameBuffer().color = Color::BLACK;
		ringu(0);
		Shader::defaultShader["texture2D"](0);
		testRenderable.params.textured = true;
		testRenderable2.params.textured = true;
		//getFrameBuffer().tint = Color::RED;
		color = Color::BLUE;
		//getFrameBuffer().transform.scale.x = 0.5;
		//getFrameBuffer().transform.scale.y = 0.5;
	}

	void onLogicFrame() override {
		/*getFrameBuffer().transform.rotation.z
			= sin(getFrameCounter()/60.0)/ 4;*/
		testRenderable.transform.local.position.z -= 0.02;
		if (testRenderable.transform.local.position.z < -25)
			testRenderable.transform.local.position.z = -5;
	}

	void onDrawBegin() override {
	}

	void onDrawEnd() override {
	}

	void onClose() override {
	}
};

#ifndef SIDE_B
int main() {
	/*
	***********************
	*                     *
	*  Main Program Code  *
	*                     *
	***********************
	*/

	GameApp prog(960, 720, "[TEST]");

	// [[ Main Program Code BEGIN ]]

	Scene::camera.eye = Vector::Vector3(0, 10, 0);
	Scene::camera.at = Vector::Vector3(0, 0, 10);
	Scene::camera.up = Vector::Vector3(0, 1, 0);

	SLF::SLFData data = SLF::parseFile("shaders/base/base.slf");
	Shader::defaultShader.destroy();
	Shader::defaultShader.create(data);

	RenderData::AnimatedPlaneReference* p[12];
	RenderData::AnimatedPlaneReference* p2[12];

	for (size_t i = 0; i < 12; i++) {
		p[i] = prog.testRenderable.createAnimatedPlaneReference();
		p[i]->local.position.z = 5*i + 5;
		p[i]->setColor(
			Color::WHITE,
			Color::WHITE,
			Color::NONE,
			Color::NONE
		);
		p[i]->size = Vector::Vector2(1, 2);
		p2[i] = prog.testRenderable2.createAnimatedPlaneReference();
		p2[i]->local.position.z = 5*i + 5;
		p2[i]->local.position.y = -5;
		p2[i]->setColor(
			Color::WHITE,
			Color::WHITE,
			Color::NONE,
			Color::NONE
		);
		p2[i]->size = Vector::Vector2(1, 2);
		p2[i]->sprite = Vector::Vector2(0, 1);
		//p2[i]->local.rotation.z = Math::pi;
	}

	p[11]->setColor(Color::PURPLE);
	p[10]->setColor(Color::MAGENTA);
	p[9]->setColor(Color::PINK);
	p[8]->setColor(Color::RED);
	p[7]->setColor(Color::ORANGE);
	p[6]->setColor(Color::YELLOW);
	p[5]->setColor(Color::LIME);
	p[4]->setColor(Color::GREEN);
	p[3]->setColor(Color::TEAL);
	p[2]->setColor(Color::CYAN);
	p[1]->setColor(Color::AZURE);
	p[0]->setColor(Color::BLUE);

	// [[ Main Program Code END ]]

	prog.run();

	return 0;
}
#else
#endif
