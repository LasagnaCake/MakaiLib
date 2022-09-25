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
	) {
		SLF::SLFData data = SLF::parseFile("shaders/base/base.slf");
		Shader::defaultShader.destroy();
		Shader::defaultShader.create(data);
	};

	GameData::PlayerEntity2D player;

	void onOpen() override {
		EntityClass::$_ROOT.addChild(&player);
	}

	void onLogicFrame() override {
		player.rotation += 1.0/60.0;
	}

	void onDrawBegin() override {
	}

	void onLayerDrawBegin(size_t layerID) override {
		switch (layerID) {
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}
	}

	void onLayerDrawEnd(size_t layerID) override {
	}

	void onDrawEnd() override {
	}

	void onClose() override {
	}
};

#ifndef SIDE_B

using namespace Vector;

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

	Scene::camera.eye	= Vector3(0,0,-10);
	Scene::camera.at	= Vector3(0,0,0);
	Scene::camera.up	= Vector3(0,1,0);

	prog.player.zIndex = 10;
	// [[ Main Program Code END ]]
	prog.run();

	return 0;
}
#else
#endif
