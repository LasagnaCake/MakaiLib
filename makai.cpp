// #include <irrlicht.h>
#include <stdexcept>

//#undef _DEBUG_OUTPUT_

#include "src/program.hpp"

#include <windows.h>

//#define SIDE_B

using namespace $rdt Reference;
using namespace Vector;
using $gdt BulletParam;

class GameApp: public $mki Program {
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
		$debug($drw layers.getGroups(&(player.mesh.render))[0]);
		maxFrameRate = 10.0;
	};

	$evt Timer bulletSpawner = $evt Timer(60, true);

	float rotAngle = 0.0;

	#define BULLET_COUNT (4096)

	$gdt PlayerEntity2D player;
	$gdt BulletManager<BULLET_COUNT, $gdt Bullet> testM;

	void setCamera2D(float scale = 64) {
		$scn camera.eye	= Vector3(0,0,-10);
		$scn camera.at	= Vector3(0,0,0);
		$scn camera.up	= Vector3(0,1,0);
		$scn camera.ortho.enabled = true;
		Vector2 screenSpace = getWindowScale();
		$scn camera.ortho.origin = 0;
		$scn camera.ortho.size = screenSpace * -scale;
	}

	void onOpen() override {
		Vector2 screenSpace = getWindowScale();
		player.spawnPoint =
		player.position =
		Vector2(32, -48) * screenSpace;
		$rdt Renderable progress;
		progress.setRenderLayer(Math::maxSizeT);
		Plane* bar = progress.createReference<Plane>();
		setCamera2D();
		float progTick = $scn camera.ortho.size.x / BULLET_COUNT;
		size_t progCount = 0;
		bar->local.scale.x = 0;
		bool forceQuit = false;
		testM.create(
			$signal {
				bar->local.scale.x += progTick;
				renderReservedLayer();
				if $event(SDL_QUIT) {
					forceQuit = true;
					testM.haltProcedure = true;
				}
			}
		);
		Vector2 screenSize = $scn camera.ortho.size.absolute();
		Vector2 screenPosition = Vector2(32, -32) * screenSpace;
		$debug(screenSize.x);
		testM.playfield = $cdt makeBounds(screenPosition, screenSize * Vector2(1.5, 2.0));
		testM.board = $cdt makeBounds(screenPosition, screenSize);
		if (forceQuit) {close(); return;}
		bar->local.scale.x = 0;
		bulletSpawner.onSignal = $signal {
			float coefficient = 0;
			Vector2 bPos = Vector2(32, -32) * getWindowScale();
			for (size_t i = 0; i < 20; i++){
				auto b = testM.createBullet();
				b->local.position = bPos;
				coefficient = (Math::tau * ((i + 1) / 20.0)) + rotAngle;
				b->settings.hitbox.radius = 1;
				b->settings.speed = BulletParam{
					-20,
					20,
					0.005
				};
				b->settings.rotation = BulletParam{
					coefficient,
					coefficient + (Math::pi * 3.0),
					0.0006/3.0
				};
				b->settings.shuttle = true;
				b->reset();
			}
			$debug(testM.getFreeCount());
			//rotAngle += (Math::pi/20.0);
		};
	}

	void onLogicFrame() override {
		//player.rotation += (2.0/maxFrameRate);
	}

	void onDrawBegin() override {
	}

	#define $rlayer(LAYER) ($layer(LAYER) / SUBLAYER_COUNT)
	void onLayerDrawBegin(size_t layerID) override {
		switch (layerID / SUBLAYER_COUNT) {
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
