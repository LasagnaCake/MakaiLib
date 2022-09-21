// #include <irrlicht.h>
#include <stdexcept>

#include "src/program.hpp"

#include <windows.h>

//#define SIDE_B

using namespace RenderData::Reference;

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
	RenderData::Renderable testRenderable3;
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
		getFrameBuffer().transform.scale.x = 0.8;
		getFrameBuffer().transform.scale.y = 0.8;
		//getFrameBuffer().tint = Color::RED;
		testRenderable.setRenderLayer(2);
		testRenderable2.setRenderLayer(1);
		testRenderable3.setRenderLayer(0);
	}

	void onLogicFrame() override {
		getFrameBuffer().transform.rotation.z
			= sin(getCurrentFrame()/60.0)/ 4;
		testRenderable.transform.local.position.z -= 0.1;
		testRenderable2.transform.local.position.z += 0.1;
		if (testRenderable.transform.local.position.z < -25)
			testRenderable.transform.local.position.z = -5;
		if (testRenderable2.transform.local.position.z > -5)
			testRenderable2.transform.local.position.z = -25;
		for(size_t i = 0; i < 12; i++)
			testRenderable.getReference<Plane>(i)->setColor(Color::hueToPastel(getCurrentFrame() / 150.0));
	}

	void onDrawBegin() override {
	}

	void onLayerDrawBegin(size_t layerID) override {
		switch (layerID) {
		case 0:
			getLayerBuffer().tint = Color::BLUE;
			getLayerBuffer().transform.position.x = sin(getCurrentFrame()/120.0) * 500;
			break;
		case 1:
			getLayerBuffer().tint = Color::GREEN;
			getLayerBuffer().transform.rotation.z = sin(getCurrentFrame()/90.0);
			break;
		case 2:
			//getLayerBuffer().tint = Color::Color(2,1,1);
			getLayerBuffer().tint = Color::WHITE;
			//getLayerBuffer().transform.rotation.z = sin(getCurrentFrame()/120.0);
			break;
		default: break;
		}
	}

	void onLayerDrawEnd(size_t layerID) override {
		getLayerBuffer().transform.rotation.z = 0;
		getLayerBuffer().transform.position.x = 0;
	}

	void onDrawEnd() override {
		getLayerBuffer().tint = Color::WHITE;
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
	prog.maxFrameRate = 60;

	Scene::camera.eye = Vector::Vector3(0, 10, 0);
	Scene::camera.at = Vector::Vector3(0, 0, 10);
	Scene::camera.up = Vector::Vector3(0, 1, 0);

	SLF::SLFData data = SLF::parseFile("shaders/base/base.slf");
	Shader::defaultShader.destroy();
	Shader::defaultShader.create(data);

	AnimatedPlane* p[12];
	AnimatedPlane* p2[12];
	Plane* p3[12];

	for (size_t i = 0; i < 12; i++) {
		// Test Planes 1
		p[i] = prog.testRenderable.createReference<AnimatedPlane>();
		p[i]->local.position.z = 5*i + 5;
		p[i]->setColor(
			Color::WHITE,
			Color::WHITE,
			Color::NONE,
			Color::NONE
		);
		p[i]->size = Vector::Vector2(1, 2);
		// Test Planes 2
		p2[i] = prog.testRenderable2.createReference<AnimatedPlane>();
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
		// Test Planes 3
		p3[i] = prog.testRenderable3.createReference<Plane>();
		p3[i]->local.position.z = 5*i + 5;
		p3[i]->local.position.y = -5;
		p3[i]->setColor(
			Color::GRAY,
			Color::GRAY,
			Color::DARKGRAY,
			Color::DARKGRAY
		);
		// Colouring Planes 1
		p[i]->setColor(Color::hueToRGB(i/12.0));
	}

	// [[ Main Program Code END ]]

	prog.run();

	return 0;
}
#else
#endif
