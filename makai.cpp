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
	$cam Camera3D cam3D{$vec3(0, 5, -10), $vec3(0, 0, 0)};

	$rdt Renderable tubeRend;

	void setCamera2D() {
		$scn camera = cam2D;
	}

	void onOpen() override {
		const size_t sideCount = 16;
		tubeRend.params.autoSort = true;
		for $ssrange(i, 0, sideCount) {
			$ref Plane* pl = tubeRend.createReference<$ref Plane>();
			$vec2
			p1 = $vmt angleV2((TAU / (float)sideCount) * (float)i),
			p2 = $vmt angleV2((TAU / (float)sideCount) * (float)(i + 1));
			$debug((TAU / (float)sideCount) * (float)i);
			$vec3
			tl = $vec3(p1, -5),
			tr = $vec3(p1, 10),
			bl = $vec3(p2, -3),
			br = $vec3(p2, 12);
			pl->setOrigin(tl, tr, bl, br);
			$vec4
			c1 = Color::hueToRGB(i/(float)sideCount) ,
			c2 = Color::hueToPastel(i/(float)sideCount)+ $vec4(.2,.2,.2,0);
			pl->setColor(c1, c2, c1, c2);
			tubeRend.unbindReference<$ref Plane>(pl);
		}
		for $ssrange(i, 0, 10) {
			$ref Plane* pl = tubeRend.createReference<$ref Plane>();
			pl->setOrigin(
				$vec3(-2, 2, 9.0 - i * 1.25),
				$vec3(2, 2, 9.0 - i * 1.25),
				$vec3(-2, -2, 9.0 - i * 1.25),
				$vec3(2, -2, 9.0 - i * 1.25)
			);
			/*pl->setOrigin(
				$vec3(-2, 2, i * 0.9),
				$vec3(2, 2, i * 0.9),
				$vec3(-2, -2, i * 0.9),
				$vec3(2, -2, i * 0.9)
			);*/
			pl->setColor($vec4(Color::SEMILUCENT).compensated());
			if (i == 10)
				pl->setColor(Color::WHITE);
			tubeRend.unbindReference<$ref Plane>(pl);//*/
		}
		tubeRend.trans.scale = $vec3($vec2(10), 2);
		tubeRend.trans.position.y = 5;
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
					$twn ease.out.linear,
					-20,
					20,
					0//.01
				};
				b->params.rot = BulletParam{
					$twn ease.out.elastic,
					coefficient,
					coefficient + (PI * 3.0),
					0.005/3.0
				};
				b->params.rebound = true;
				b->pause.time = 30;
				b->pause.enabled = true;
				b->reset();
			}
			$debug(ebm.getFreeCount());
			//rotAngle += (PI/20.0);
		};
		bulletSpawner.stop();
	}

	void onLogicFrame() override {
		//player.rotation += (2.0/maxFrameRate);
	}

	void onDrawBegin() override {
		cam3D.eye.x	= sin(getCurrentFrame() / 60.0) * 3.0;
		cam3D.at.y	= cos(getCurrentFrame() / 60.0) * 3.0;
		tubeRend.trans.rotation.z = getCurrentFrame() / 180.0;
		tubeRend.trans.position.z = cos(getCurrentFrame() / 120.0) * 10;
	}

	#define $rlayer(LAYER) ($layer(LAYER) / SUBLAYER_COUNT)
	void onLayerDrawBegin(size_t layerID) override {
		switch (layerID / SUBLAYER_COUNT) {
		case $rlayer(WORLD):
			$scn camera = cam3D;
			//getLayerBuffer().tint = Color::hueToPastel(getCurrentFrame() / 240.0);
			//getLayerBuffer().tint = Color::hueToRGB(getCurrentFrame() / 120.0);
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
