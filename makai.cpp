// #include <irrlicht.h>
#include <stdexcept>

//#undef _DEBUG_OUTPUT_

#include "src/program.hpp"

#include <windows.h>

//#define SIDE_B

using namespace RenderData::Reference;
using namespace Vector;
using GameData::BulletParam;

class GameApp: public Makai::Program {
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
		$mainshader.destroy();
		$mainshader.create(data);
		player.mesh.setRenderLayer($layer(PLAYER));
		$debug(Drawer::layers.getGroups(&(player.mesh.render))[0]);
	};

	$gdt PlayerEntity2D player;
	$gdt BulletManager<4096> testM;

	void setCamera2D(float scale = 64) {
		$scn camera.eye	= Vector3(0,0,-10);
		$scn camera.at	= Vector3(0,0,0);
		$scn camera.up	= Vector3(0,1,0);
		$scn camera.ortho.enabled = true;
		Vector2 screenSpace = getWindowSize().normalized();
		$scn camera.ortho.origin = 0;
		$scn camera.ortho.size = screenSpace * -scale;
	}

	void onOpen() override {
		Vector2 screenSpace = getWindowSize().normalized();
		player.spawnPoint =	Vector2(32, -16);
		player.position =	Vector2(32, -32);
		//player.position *= screenSpace;
		$rdt Renderable progress;
		progress.setRenderLayer(Math::maxSizeT);
		Plane* bar = progress.createReference<Plane>();
		setCamera2D();
		float progTick = $scn camera.ortho.size.x / 4096.0;
		size_t progCount = 0;
		bar->local.scale.x = 0;
		auto bulletSignal = $signal {
			bar->local.scale.x += progTick;
			renderReservedLayer();
			Makai::pollEvents();
		};
		testM.onBulletCreated = bulletSignal;
		testM.create();
		bar->local.scale.x = 0;
		for (size_t i = 0; i < 32; i++){
			auto b = testM.createBullet();
			b->local.position = Vector2(
				16, -16
			);
			b->settings.hitbox.radius = 1;
			b->settings.speed = BulletParam{0, 5, 0.05};
			b->settings.rotation.start = 6.28 * (i / 32.0);
			b->reset();
			bar->local.scale.x += progTick * (4096/256);
			renderReservedLayer();
			Makai::pollEvents();
		}
	}

	void onLogicFrame() override {
		//player.rotation += (2.0/maxFrameRate);
	}

	void onDrawBegin() override {
	}

	#define $rlayer(LAYER) ($layer(LAYER) / 8)
	void onLayerDrawBegin(size_t layerID) override {
		switch (layerID / 8) {
		case $rlayer(WORLD):
			break;
		case $rlayer(PLAYER):
			break;
		case $rlayer(UI):
			break;
		default:
			break;
		}
	}
	#undef $rlayer

	void onLayerDrawEnd(size_t layerID) override {
		setCamera2D();
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
	#ifndef _DEBUG_OUTPUT_
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	#endif // _DEBUG_OUTPUT_

	GameApp prog(960, 720, "[TEST]");

	// [[ Main Program Code BEGIN ]]
	prog.maxFrameRate = 60;
	// [[ Main Program Code END ]]
	prog.run();
	return 0;
}
#else
#endif
