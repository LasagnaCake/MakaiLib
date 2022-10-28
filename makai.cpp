// #include <irrlicht.h>
#include <stdexcept>

//#undef _DEBUG_OUTPUT_

#define $_PREVENT_BULLET_OVERFLOW_BY_WRAP

#include "src/makai.hpp"

#include <windows.h>

//#define SIDE_B

using namespace $rdt Reference;
using namespace Vector;
using $dmk ObjectParam;

class DanmakuApp: public $mki Program {
public:
	DanmakuApp(
		unsigned int width,
		unsigned int height,
		std::string windowTitle,
		bool fullscreen = false,
		std::string bufferShaderPath = "shaders/framebuffer/compose.slf",
		std::string mainShaderPath = "shaders/base/base.slf"
	) : Program (
			width,
			height,
			windowTitle,
			fullscreen,
			bufferShaderPath
	) {
		// Create main shader
		SLF::SLFData data = SLF::parseFile(mainShaderPath);
		$mainshader.destroy();
		$mainshader.create(data);
		// Set bullet managers
		$setb(enemy)	ebm;
		$setll(enemy)	ellm;
	};

	$bullet(Enemy)		ebm;
	$linelaser(Enemy)	ellm;

	$cam Camera3D cam2D;
	$cam Camera3D cam3D{$vec3(0, 5, -10), $vec3(0, 0, 0)};

	size_t gameSeed = 0;

	void setCamera2D() {
		$scn camera = cam2D;
	}

	void setCamera3D() {
		$scn camera = cam3D;
	}

	void onOpen() override {
		// Set RNG Seed
		gameSeed = $rng getNewSeed();
		$debug(gameSeed);
		$rng setSeed(gameSeed);
		// Create 2D Camera
		Vector2 screenSpace = getWindowScale();
		cam2D = $cam getCamera2D(64, screenSpace);
		// Set 2D Camera
		setCamera2D();
		// Create thread for processing input
		bool doneCreating = false;
		auto subTask = [&](){
			while (!doneCreating) {
				if $event(SDL_QUIT) {
					ebm.haltProcedure	= true;
					ellm.haltProcedure	= true;
					close();
				}
			}
		};
		std::thread secondary(subTask);
		// Create things
		ebm.create();
		ellm.create();
		doneCreating = true;
		secondary.join();
		// Set playfield
		Vector2 screenSize = $scn camera.ortho.size.absolute();
		Vector2 screenPosition = Vector2(32, -32) * screenSpace;
		ebm.playfield = $cdt makeBounds(screenPosition, screenSize * Vector2(1.5, 2.0));
		ebm.board = $cdt makeBounds(screenPosition, screenSize);
	}

	#define $rlayer(LAYER) ($layer(LAYER) / SUBLAYER_COUNT)
	void onLayerDrawBegin(size_t layerID) override {
		switch (layerID / SUBLAYER_COUNT) {
		case $rlayer(WORLD):
			setCamera3D();
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
};

class GameApp: public DanmakuApp {
public:
	GameApp(
		unsigned int width,
		unsigned int height,
		std::string windowTitle,
		bool fullscreen = false
	) : DanmakuApp (
			width,
			height,
			windowTitle,
			fullscreen,
			"shaders/framebuffer/compose.slf",
			"shaders/base/base.slf"
	) {

	}

	$evt Timer bulletSpawner = $evt Timer(60, true);

	float rotAngle = 0.0;

	$dmk PlayerEntity2D	player;

	$rdt Renderable		tubeRend;

	void onOpen() override {
		// Do parent task
		DanmakuApp::onOpen();
		// Create background
		const size_t sideCount = 16;
		$ref Plane* pl = tubeRend.createReference<$ref Plane>();
		pl->setOrigin(
			$vec3(-2, 2, 12.5),
			$vec3(2, 2, 12.5),
			$vec3(-2, -2, 12.5),
			$vec3(2, -2, 12.5)
		);
		pl->setColor($vec4(0,0,0.5,1));
		tubeRend.unbindReference<$ref Plane>(pl);//*/
		for $ssrange(i, 0, sideCount) {
			pl = tubeRend.createReference<$ref Plane>();
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
			pl = tubeRend.createReference<$ref Plane>();
			pl->setOrigin(
				$vec3(-2, 2, 9.0 - i * 1.25),
				$vec3(2, 2, 9.0 - i * 1.25),
				$vec3(-2, -2, 9.0 - i * 1.25),
				$vec3(2, -2, 9.0 - i * 1.25)
			);
			pl->setColor($vec4(Color::SEMILUCENT).compensated());
			tubeRend.unbindReference<$ref Plane>(pl);//*/
		}
		tubeRend.trans.scale = $vec3($vec2(10), 2);
		tubeRend.trans.position.y = 5;
		// Set player stuff
		Vector2 screenSpace = getWindowScale();
		player.spawnPoint =
		player.position =
		Vector2(32, -48) * screenSpace;
		player.grazebox.radius = 2.5;
		// Create test bullet spawner
		bulletSpawner.onSignal = $signal {
			float coefficient = 0;
			Vector2 bPos = Vector2(32, -32) * getWindowScale();
			for (size_t i = 0; i < 20; i++){
				auto b = ebm.createBullet();
				b->local.position = bPos;
				coefficient = (Math::tau * ((i + 1) / 20.0)) + rotAngle;
				b->params.hitbox.radius = 1;
				b->params.vel = ObjectParam{
					$twn ease.out.linear,
					-20,
					20,
					0//.01
				};
				b->params.rot = ObjectParam{
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
			//rotAngle += (PI/20.0);
		};
		// Create test laser A
		Vector2 lPos = Vector2(32, -16) * getWindowScale();
		auto l = ellm.createLineLaser();
		l->local.position = lPos;
		l->params.rot = ObjectParam {
			$twn ease.inOut.back,
			-PI,
			0,
			0.005
		};
		l->params.length.start = 30;
		l->params.width.start = 2;
		l->params.active = true;
		l->params.discardable = false;
		l->reset();
		// Test laser B
		l = ellm.createLineLaser();
		l->local.position = lPos;
		l->params.rot = ObjectParam {
			$twn ease.inOut.back,
			0,
			-PI,
			0.005
		};
		l->params.length.start = 30;
		l->params.width.start = 2;
		l->params.active = true;
		l->params.discardable = false;
		l->reset();
		//bulletSpawner.stop();
	}

	void onLogicFrame() override {
		//player.rotation += (2.0/maxFrameRate);
	}

	void onDrawBegin() override {
		float fac = (0.5 + (cos(getCurrentFrame() / 120.0) / 2.0));
		cam3D.eye.x	= sin(getCurrentFrame() / 60.0) * 3.0;
		cam3D.at.y	= cos(getCurrentFrame() / 60.0) * 3.0;
		tubeRend.trans.rotation.z = getCurrentFrame() / 180.0;
		tubeRend.trans.position.z = Math::lerp(-20.0f, 10.0f, fac);
	}

	#define $rlayer(LAYER) ($layer(LAYER) / SUBLAYER_COUNT)
	void onLayerDrawBegin(size_t layerID) override {
		DanmakuApp::onLayerDrawBegin(layerID);
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
		DanmakuApp::onLayerDrawEnd(layerID);
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
