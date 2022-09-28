// #include <irrlicht.h>
#include <stdexcept>

#include "src/program.hpp"

#include <windows.h>

//#define SIDE_B

using namespace RenderData::Reference;
using namespace Vector;

void setCamera2D() {
	Scene::camera.eye	= Vector3(0,0,-10);
	Scene::camera.at	= Vector3(0,0,0);
	Scene::camera.up	= Vector3(0,1,0);
}

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
		EntityClass::$_ROOT.addChild(&player);
		player.mesh.setRenderLayer($layer(PLAYER));
		$debug(Drawer::layers.getGroups(&(player.mesh.render))[0]);
	};

	GameData::PlayerEntity2D player;

	void onOpen() override {
	}

	void onLogicFrame() override {
		player.rotation += 1.0/60.0;
	}

	void onDrawBegin() override {
	}

	#define $rlayer(LAYER) ($layer(LAYER) / 8)
	void onLayerDrawBegin(size_t layerID) override {
		switch (layerID / 8) {
		case $rlayer(WORLD):
			break;
		case $rlayer(PLAYER):
			setCamera2D();
			getLayerBuffer().tint = Color::RED;
			break;
		case $rlayer(UI):
			break;
		default:
			break;
		}
	}
	#undef $rlayer

	void onLayerDrawEnd(size_t layerID) override {
		getLayerBuffer().tint = Color::WHITE;
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
	prog.maxFrameRate = 60;

	prog.player.zIndex = 10;
	// [[ Main Program Code END ]]
	prog.run();

	return 0;
}
#else
#endif
