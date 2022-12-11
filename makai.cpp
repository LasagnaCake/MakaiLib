//#undef _DEBUG_OUTPUT_

#define $_PREVENT_BULLET_OVERFLOW_BY_WRAP
#define $_DANMAKU_GAME

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
	) : DanmakuApp (
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

	$rdt Renderable		tubeRend;

	$rdt Text::Label	testLabel;

	void onOpen() override {
		// British? 😩
		// maxFrameRate = 50;
		// maxFrameRate = 60;
		// maxFrameRate = 20;
		// Do parent task
		DanmakuApp::onOpen();
		// Create background
		const size_t sideCount = 16;
		$ref Plane* pl = tubeRend.createReference<$ref Plane>();
		$debug("Backplane Created!");
		pl->setOrigin(
			$vec3(-2, 2, 12.5),
			$vec3(2, 2, 12.5),
			$vec3(-2, -2, 12.5),
			$vec3(2, -2, 12.5)
		);
		testLabel.setRenderLayer(Layer::UI_LAYER);
		testLabel.font.face = new Drawer::Texture2D("img/fonts/font.png");
		testLabel.trans.scale = 2.5;
		//pl->setColor($vec4(0,0,0.5,1));
		pl->unbind();//*/
		for $ssrange(i, 0, sideCount) {
			$debugp(sideCount);
			$debug(" Strip created!");
			// Set 1
			pl = tubeRend.createReference<$ref Plane>();
			$vec2
			p1 = $vmt angleV2((TAU / (float)sideCount) * (float)i),
			p2 = $vmt angleV2((TAU / (float)sideCount) * (float)(i + 1)),
			p3 = $vmt angleV2((TAU / (float)sideCount) * (float)(i + 2)),
			p4 = $vmt angleV2((TAU / (float)sideCount) * (float)(i + 3));
			$debug((TAU / (float)sideCount) * (float)i);
			$vec3
			tl = $vec3(p1, -5),
			tr = $vec3(p2, 2.5),
			bl = $vec3(p2, -3),
			br = $vec3(p3, 4.5);
			pl->setOrigin(tl, tr, bl, br);
			$vec4
			c1 = Color::hueToRGB(i/(float)sideCount),
			c2 = Color::hueToPastel(i/(float)sideCount)+ $vec4(.2,.2,.2,0);
			pl->setColor(c1, c2, c1, c2);
			pl->unbind();
			// Set 2
			pl = tubeRend.createReference<$ref Plane>();
			$debug((TAU / (float)sideCount) * (float)i);
			tl = $vec3(p2, 2.5),
			tr = $vec3(p3, 10),
			bl = $vec3(p3, 4.5),
			br = $vec3(p4, 12);
			pl->setOrigin(tl, tr, bl, br);
			c1 = Color::hueToRGB(i/(float)sideCount),
			c2 = Color::hueToPastel(i/(float)sideCount)+ $vec4(.2,.2,.2,0);
			pl->setColor(c1, c2, c1, c2);
			pl->unbind();
		}
		$debug("\n [ BACKGROUND DONE ] \n");
		/*
		for $ssrange(i, 0, 10) {
			pl = tubeRend.createReference<$ref Plane>();
			pl->setOrigin(
				$vec3(-2, 2, 9.0 - i * 1.25),
				$vec3(2, 2, 9.0 - i * 1.25),
				$vec3(-2, -2, 9.0 - i * 1.25),
				$vec3(2, -2, 9.0 - i * 1.25)
			);
			pl->setColor($vec4(Color::SEMILUCENT).compensated());
			pl->unbind();
		}*/
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
		world3D.farFog = {true, 20, 10, $vec3(0, 0, 0.2)};
		world3D.ambient.color = Vector3(1,1,1);
	}

	void onLogicFrame(float delta) override {
		if (input.getButtonDown(SDL_SCANCODE_ESCAPE)) {
			close();
		}
	}

	void onDrawBegin() override {
		float fac = (0.5 + (cos(getCurrentFrame() / (maxFrameRate * 2.0)) / 2.0));
		cam3D.eye.x	= sin(getCurrentFrame() / maxFrameRate) * 3.0;
		cam3D.at.y	= cos(getCurrentFrame() / maxFrameRate) * 3.0;
		tubeRend.trans.rotation.z = getCurrentFrame() / (maxFrameRate * 3.0);
		tubeRend.trans.position.z = Math::lerp(-20.0f, 10.0f, fac);
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
			layerMaterial.background = Vector4(0, 0, 0.2, 1);
			layerMaterial.wave = {
				true,
				$vec2(5, -5),
				$vec2(
					Math::min(sin(getCurrentFrame()/40.0) * 0.5, 0.0),
					Math::min(sin(getCurrentFrame()/40.0) * -0.5, 0.0)
				),
				$vec2(getCurrentFrame()/60.0)
			};
			break;
		default:
			break;
		}
	}

	void onLayerDrawEnd(size_t layerID) override {
		setCamera2D();
		setWorldMaterial2D();
		auto& layerMaterial = getLayerBuffer().material;
		layerMaterial.background = Color::CLEAR;
		layerMaterial.wave = {};
		layerMaterial.color = Color::WHITE;
	}

	void onDrawEnd() override {
	}

	void onClose() override {
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
	} catch (std::runtime_error e) {
		Popup::errorDialog(e.what());
	}
	#ifdef _DEBUG_OUTPUT_
	$debug("\nAll done! Please press enter to close program!");
	getchar();
	#endif // _DEBUG_OUTPUT_
	return 0;
}
