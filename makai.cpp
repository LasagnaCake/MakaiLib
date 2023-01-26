//#undef _DEBUG_OUTPUT_

#define $_PREVENT_BULLET_OVERFLOW_BY_WRAP
#define $_PARALLEL_MANAGERS
//#define $_PARALLEL_RENDERING
#define $_DANMAKU_GAME
#define $_FRAME_INDEPENDENT_PROCESS
//#define $_PROCESS_RENDER_BEFORE_LOGIC

#define RADIAL_BAR_RESOLUTION 12

#include "src/makai.hpp"

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <wtypes.h>
#include <windows.h>
#include <mmsystem.h>
#endif

using namespace $rdt Reference;
using namespace Vector;
using $dmk ObjectParam;
using $dmk CollectibleData;

class GameApp: public $dmk DanmakuApp {
public:
	GameApp(
		unsigned int width,
		unsigned int height,
		std::string windowTitle,
		bool fullscreen = false
	): DanmakuApp (
			width,
			height,
			windowTitle,
			fullscreen,
			"shaders/framebuffer/compose.slf",
			"shaders/base/base.slf"
		) {

	}

	$evt Timer bulletSpawner = $evt Timer(5, true);

	float rotAngle = 0.0;

	$dmk PlayerEntity2D	player;

	$dmk EnemyEntity2D*	enemy;

	$rdt Renderable*	tubeRend;

	$txt Label			testLabel;

	$txt FontData		font{new Drawer::Texture2D("img/fonts/fontGRID.png"), $vec2(16), $vec2(0.55, 1.0)};

	$drw Texture2D*		ringbar = new Drawer::Texture2D("img/ring.png");

	void onOpen() override {
		// British? 😩
		// maxFrameRate = 50;
		// maxFrameRate = 60;
		// maxFrameRate = 20;
		// Do parent task
		DanmakuApp::onOpen();
		// Create background
		tubeRend = $rdt loadObjectFromFile("stages/test.bin");
		tubeRend->trans.scale = $vec3($vec2(10), 2);
		tubeRend->trans.position.y = 5;
		//tubeRend->material.fill = GL_FILL;
		// Set player stuff
		Vector2 screenSpace = getWindowScale();
		player.spawnPoint =
		player.position =
		Vector2(32, -48) * screenSpace;
		enemy = new $dmk EnemyEntity2D("Test");
		enemy->position = Vector2(32, -24) * screenSpace;
		enemy->sprite->setColor(Color::RED);
		//enemy->healthBar.offset.x = 0.5;
		enemy->healthBar.material.texture.enabled	= true;
		enemy->healthBar.centered = true;
		enemy->healthBar.material.texture.image		= ringbar;
		//enemy->healthBar.dynamicUV = false;
		player.grazebox.radius = 2.5;
		// Create test bullet spawner
		bulletSpawner.onSignal = $signal {
			float coefficient = 0;
			Vector2 bPos = Vector2(32, -32) * getWindowScale();
			for (size_t i = 0; i < 5; i++){
				auto b = DANMAKU_EBM -> createBullet();
				b->local.position = bPos;
				coefficient = (Math::tau * ((i + 1) / 5.0)) + rotAngle;
				b->params.hitbox.radius = 1;
				b->params.vel = {
					$twn ease.out.linear,
					-20,
					20,
					0//.01
				};
				b->params.rot = {
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
			rotAngle += (PI/5.0);
		};
		testLabel.setRenderLayer($layer(WORLD));
		testLabel.font = &font;
		testLabel.trans.scale = 2.5;
		testLabel.trans.scale.x *= 0.8;
		testLabel.text.content = "   Spell shattered!\n(Get spell card bonus)";
		testLabel.trans.position = $vec3(10, 10, 2.5);
		testLabel.trans.rotation.y = PI;
		// Create test laser A
		Vector2 lPos = Vector2(32, -16) * getWindowScale();
		auto l = DANMAKU_ELLM -> createLineLaser();
		l->local.position = lPos;
		l->params.rot = {
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
		l = DANMAKU_ELLM -> createLineLaser();
		l->local.position = lPos;
		l->params.rot = {
			$twn ease.inOut.back,
			0,
			-PI,
			0.005
		};
		l->params.length.start = 30;
		l->params.width.start = 2;
		//l->params.active = true;
		l->params.discardable = false;
		l->reset();
		bulletSpawner.stop();
		player.board = DANMAKU_EBM -> board;
		DANMAKU_IM -> createCollectible(CollectibleData(), 5, lPos, 3, $vec2(0.5));
		world3D.farFog = {true, 20, 10, $vec4(0, 0, .2, 1)};
		world3D.ambient.color = Vector3(1,1,1);
		player.mesh.material.color = Color::GREEN;
		enemy->setInvincible(120);
	}

	void onLogicFrame(float delta) override {
		if (input.getButtonDown(SDL_SCANCODE_ESCAPE)) {
			close();
		}
	}

	void onDrawBegin() override {
		float fac = (0.5 + (cos(getCurrentCycle() / (maxCycleRate * 2.0)) / 2.0));
		cam3D.eye.x	= sin(getCurrentCycle() / maxCycleRate) * 3.0;
		cam3D.at.y	= cos(getCurrentCycle() / maxCycleRate) * 3.0;
		tubeRend->trans.rotation.z = getCurrentCycle() / (maxCycleRate * 3.0);
		tubeRend->trans.position.z = Math::lerp(-20.0f, 10.0f, fac);
	}

	#define $rlayer(LAYER) ($layer(LAYER) / SUBLAYER_COUNT)
	void onLayerDrawBegin(size_t layerID) override {
		switch (layerID / SUBLAYER_COUNT) {
		case $rlayer(WORLD):
			setCamera3D();
			setWorldMaterial3D();
			onWorldSublayer(Math::wmax(layerID, (size_t)8));
			break;
		case $rlayer(PLAYER):
			break;
		case $rlayer(ENEMY):
			break;
		case $rlayer(OVERLAY):
			/*getLayerBuffer().material.outline.enabled = true;
			getLayerBuffer().material.outline.size = 0.005;
			getLayerBuffer().material.outline.relativeAlpha = false;*/
			break;
		case $rlayer(UI):
			break;
		default:
			break;
		}
	}
	#undef $rlayer

	void onWorldSublayer(size_t sublayerID) {
		auto& layerMaterial = getLayerBuffer().material;
		switch(sublayerID) {
		case 0:
			layerMaterial.background = $vec4(0, 0, .2, 1);
			layerMaterial.plasma = {
				true,
				$vec2(3, -5),
				$vec2(
					Math::min(sin(getCurrentCycle()/40.0) * 0.5, 0.0),
					Math::min(sin(getCurrentCycle()/40.0) * -0.5, 0.0)
				),
				$vec2(getCurrentCycle()/60.0)
			};
			break;
		default:
			break;
		}
	}

	void onLayerDrawEnd(size_t layerID) override {
		setCamera2D();
		setWorldMaterial2D();
		getLayerBuffer().material = $mat BufferMaterial{};
		/*auto& layerMaterial = getLayerBuffer().material;
		layerMaterial.background = Color::CLEAR;
		layerMaterial.plasma = {};
		layerMaterial.color = Color::WHITE;*/
	}

	void onDrawEnd() override {
	}

	void onClose() override {
		DanmakuApp::onClose();
		$debug("[ Executing Closing Procedure NOW ]\n");
		delete tubeRend;
		delete font.face;
	}
};

int main() {
	/*
	***********************
	*                     *
	*  Main Program Code  *
	*                     *
	***********************
	*/
	#ifndef _DEBUG_OUTPUT_
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	#endif // _DEBUG_OUTPUT_
	try {
		// This is required, else the resolution gets messed up by window's scaling
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
		SetProcessDPIAware();
		#endif
		auto prefs = $dmk queryProgramSettingsFromUser(true);
		// If size >= screen size, automatic fullscreen for some reason (Why, SDL?)
		GameApp prog(prefs.resolution.x, prefs.resolution.y, "[TEST]", prefs.fullscreen);
		prog.maxFrameRate = prefs.framerate;
		prog.run();
		$debug("Program done running!\n");
	} catch (std::runtime_error e) {
		Popup::errorDialog(e.what());
	}
	#ifdef _DEBUG_OUTPUT_
	$debug("\nAll done! Please press enter to close program!");
	getchar();
	#endif // _DEBUG_OUTPUT_
	return 0;
}
