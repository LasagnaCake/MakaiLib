// #include <irrlicht.h>
#include <stdexcept>

//#undef _DEBUG_OUTPUT_

#define $_PREVENT_BULLET_OVERFLOW_BY_WRAP

#include "src/makai.hpp"

#include <windows.h>

//#define SIDE_B

using namespace $rdt Reference;
using namespace Vector;
using $dmk BulletParam;

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
		$setb(enemy) ebm;
		maxFrameRate = 10.0;
	};

	$evt Timer bulletSpawner = $evt Timer(60, true);

	float rotAngle = 0.0;

	$dmk PlayerEntity2D player;

	$rdt Texture2D		fontTX;

	$bullet(Enemy) ebm;

	$cam Camera3D cam2D;
	$cam Camera3D cam3D{$vec3(0, 5, 0), $vec3(0, 0, 10)};

	void setCamera2D() {
		$scn camera = cam2D;
	}

	void onOpen() override {
		size_t gameSeed = $rng getNewSeed();
		$debug(gameSeed);
		$rng setSeed(gameSeed);
		fontTX.create("FT_Set1-Lotuscoder.png");
		Vector2 screenSpace = getWindowScale();
		player.spawnPoint =
		player.position =
		Vector2(32, -48) * screenSpace;
		cam2D = $cam getCamera2D(64, screenSpace);
		player.grazebox.radius = 2.5;
		$rdt Renderable progress;
		progress.setRenderLayer(Math::maxSizeT);
		Plane* bar = progress.createReference<Plane>();
		setCamera2D();
		float progTick = $scn camera.ortho.size.x / (ENEMY_BULLET_COUNT * 1.0);
		size_t progCount = 0;
		bar->local.scale.x = 0;
		bool forceQuit = false;
		ebm.create(
			$signal {
				bar->local.scale.x += progTick;
				renderReservedLayer();
				if $event(SDL_QUIT) {
					forceQuit = true;
					ebm.haltProcedure = true;
				}
			}
		);
		Vector2 screenSize = $scn camera.ortho.size.absolute();
		Vector2 screenPosition = Vector2(32, -32) * screenSpace;
		$debug(screenSize.x);
		ebm.playfield = $cdt makeBounds(screenPosition, screenSize * Vector2(1.5, 2.0));
		ebm.board = $cdt makeBounds(screenPosition, screenSize);
		if (forceQuit) {close(); return;}
		bar->local.scale.x = 0;
		bulletSpawner.onSignal = $signal {
			float coefficient = 0;
			Vector2 bPos = Vector2(32, -32) * getWindowScale();
			for (size_t i = 0; i < 20; i++){
				auto b = ebm.createBullet();
				b->local.position = bPos;
				coefficient = (Math::tau * ((i + 1) / 20.0)) + rotAngle;
				b->params.hitbox.radius = 1;
				b->params.vel = BulletParam{
					-20,
					20,
					0.005
				};
				b->params.rot = BulletParam{
					coefficient,
					coefficient + (PI * 3.0),
					0.0006/3.0
				};
				b->params.rebound = true;
				b->pause.time = 30;
				b->pause.enabled = true;
				b->reset();
			}
			$debug(ebm.getFreeCount());
			//rotAngle += (PI/20.0);
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
