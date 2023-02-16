//#undef _DEBUG_OUTPUT_

#define $_PREVENT_BULLET_OVERFLOW_BY_WRAP
//#define $_PARALLEL_MANAGERS
//#define $_PARALLEL_RENDERING
#define $_DANMAKU_GAME
#define $_FRAME_INDEPENDENT_PROCESS
//#define $_PROCESS_RENDER_BEFORE_LOGIC
//#define $_REDUCED_GAME_INSTANCE_COUNT

#define RADIAL_BAR_RESOLUTION	12
//#define ENEMY_BULLET_COUNT		256
//*
//#define ENEMY_BULLET_COUNT		1024
//#define ENEMY_BULLET_COUNT		1536
//#define ENEMY_LASER_COUNT		32
//*/

#include "src/makai.hpp"

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <winnt.h>
#include <winuser.h>
#include <powrprof.h>
constexpr struct {
	const size_t performance	= 0xA1841308;
	const size_t balanced		= 0x381B4222;
	const size_t saver			= 0x8C5E7FDA;
} powerScheme;
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
		$debug(EntityClass::$_ROOT != nullptr);
	}

	$evt Timer bulletSpawner = $evt Timer(5, true);

	float rotAngle = 0.0;

	$dmk PlayerEntity2D	player;

	$dmk EnemyEntity2D*	enemy;

	$rdt Renderable*	tubeRend;

	$txt Label			testLabel;
	$txt Label			frameLabel;

	$txt FontData		font{new Drawer::Texture2D("img/fonts/fontGRID.png"), $vec2(16), $vec2(0.55, 0.9)};

	$drw Texture2D*		ringbar = new Drawer::Texture2D("img/ring.png");

	uint64 ticks = 0;

	void onOpen() override {
		// British? 😩
		// maxFrameRate = 50;
		// maxFrameRate = 60;
		// maxFrameRate = 20;
		// Do parent task
		$debug(EntityClass::$_ROOT != nullptr);
		DanmakuApp::onOpen();
		// Create background
		tubeRend = $rdt loadObjectFromBinaryFile("stages/test.bin");
		tubeRend->trans.scale = $vec3($vec2(10), 2);
		tubeRend->trans.position.y = 5;
		//tubeRend->material.fill = GL_LINE;
		// Set player stuff
		Vector2 screenSpace = getWindowScale();
		player.spawnPoint =
		player.position =
		Vector2(32, -48) * screenSpace;
		player.grazebox.radius = 2.5;
		player.board = DANMAKU_EBM -> board;
		player.mesh.material.color = Color::GREEN;
		enemy = new $dmk EnemyEntity2D("Test");
		enemy->position = Vector2(32, -24) * screenSpace;
		enemy->sprite->setColor(Color::RED);
		enemy->healthBar.offset.y = 0.5;
		enemy->healthBar.material.texture.enabled	= true;
		enemy->healthBar.centered = true;
		enemy->healthBar.material.texture.image		= ringbar;
		//enemy->healthBar.dynamicUV = false;
		$tsk Tasker* et = new $tsk Tasker(
			$tsk TaskList{
				$task {
					//enemy->healthBar.uvAngle += 1.0/(120.0 * (enemy->health / enemy->maxHealth));
					enemy->healthBar.uvAngle += 1.0/(120.0);
					enemy->healthBar.offset = $vmt angleV2($vmt angleTo(enemy->position, player.position)) / 2.0;
					$end;
				}
			}, true
		);
		enemy->taskers.addTasker(et);
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
					-25,
					20,
					0.005/5.0
				};
				b->params.rot = {
					$twn ease.out.elastic,
					coefficient,
					coefficient + (PI * 6.0),
					0.005/6.0
				};
				b->params.rebound = true;
				b->pause.time = 30;
				b->pause.enabled = true;
				b->reset();
			}
			rotAngle += (PI/30.0);
		};
		testLabel.setRenderLayer($layer(WORLD));
		testLabel.font = &font;
		testLabel.trans.scale = 2.5;
		testLabel.trans.scale.x *= 0.8;
		testLabel.text.rect.h = 10;
		testLabel.text.content = "Spell\nCard shattered!\n(Get spell card bonus)";
		testLabel.text.align.h = $txt HAlign::CENTER;
		testLabel.trans.position = $vec3(4, 10, 2.5);
		testLabel.trans.rotation.y = PI;
		// Frame Label
		frameLabel.setRenderLayer($layer(UI));
		frameLabel.font = &font;
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
		DANMAKU_IM -> createCollectible(CollectibleData(), 5, lPos, 3, $vec2(0.5));
		world3D.farFog = {true, 20, 10, $vec4(0, 0, .2, 1)};
		world3D.ambient.color = Vector3(1,1,1);
		enemy->setInvincible(120);
	}

	size_t currentWave = 0;

	bool useWave = true;

	void onLogicFrame(float delta) override {
		frameLabel.text.content = std::to_string(SDL_GetTicks() - ticks);
		ticks = SDL_GetTicks();
		if (input.getButtonDown(SDL_SCANCODE_ESCAPE)) {
			close();
		}
		for $ssrange(i, 0, 9) {
			if (input.isButtonJustPressed((SDL_Scancode)(SDL_SCANCODE_1 + i)))
				currentWave = i;
		}
		if (input.isButtonJustPressed(SDL_SCANCODE_0))
			useWave = !useWave;
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
		case 0: {/*
				layerMaterial.debug = {
					true,
					$mat DEBUG_VIEW_NORMAL
				};*/
				layerMaterial.background = $vec4(0, 0, .2, 1);
				$mat WaveEffect wfx = {
					true,
					$vec2(3, -5),
					$vec2(
						sin(getCurrentCycle()/40.0) * 0.5,
						cos(getCurrentCycle()/40.0) * 0.5
					),
					$vec2(getCurrentCycle()/60.0),
					currentWave
				};
				if (useWave)	layerMaterial.wave	= wfx;
				else			layerMaterial.prism	= wfx;
			} break;
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
	try {
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
		// This is required, else the resolution gets messed up by window's scaling
		GUID* scheme;
		SetProcessDPIAware();
		PowerGetActiveScheme(0, &scheme);
		switch(scheme->Data1) {
		case powerScheme.saver:
			throw std::runtime_error(
				"This application was not designed"
				"\nfor use with power saving enabled!"
				"\n\nPlease set profile to 'performance'."
			);
			break;
		case powerScheme.balanced:
			Popup::dialogBox(
				"Warning!",
				"This application was not designed"
				"\nfor use with power saving enabled!"
				"\n\nPlease ensure your power profile is"
				"\nset to 'performance'."
				"\n\n(This can be done via the battery icon.)",
				Popup::Option::OK,
				SDL_MESSAGEBOX_WARNING
			);
		default:
			break;
		}
		#endif/*
		Popup::dialogBox(
			"30FPS is not runing as intended.",
			"The 30FPS option is not running correctly."
			"\nPlease avoid using it.",
			Popup::Option::OK,
			SDL_MESSAGEBOX_INFORMATION
		);*/
		auto prefs = $dmk queryProgramSettingsFromUser(true/*, false*/);
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
