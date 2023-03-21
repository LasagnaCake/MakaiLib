#define $_PREVENT_BULLET_OVERFLOW_BY_WRAP
#define $_DANMAKU_GAME
#define $_FRAME_INDEPENDENT_PROCESS

#define RADIAL_BAR_RESOLUTION	12

#define ENEMY_BULLET_COUNT		1024

#include <makai.hpp>

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <winnt.h>
#include <winuser.h>
#include <powrprof.h>
#endif

using
	GameData::Danmaku::Stage,
	RenderData::Renderable,
	RenderData::PointRenderable,
	Drawer::RawVertex,
	Drawer::Texture2D,
	Camera::Camera3D,
	GameData::Animation::AnimationPlayer,
	Drawer::FrameBuffer
;

typedef GameData::Danmaku::DanmakuApp AppType;

class TestStage: public Stage {
public:
	GAME_CLASS(TestStage, Stage)

	Renderable	lamps;
	Renderable	ground;
	Renderable	lights;
	Renderable	lightSource;

	Texture2D	lampTexture;
	Texture2D	groundTexture;

	AnimationPlayer<Vec3>* stage;

	AppType*		app = (AppType*)GameData::getMainProgram();
	Camera3D*		camera = nullptr;

	void onCreate() override {
		// Animator
		stage = new AnimationPlayer<Vec3>();
		addChild(stage);
		// Textures
		lampTexture.create("demo/EX_Stage/LampTexture.png");
		groundTexture.create("demo/EX_Stage/GroundTexture.png");
		// Set lamp
		lamps.extendFromBinaryFile("demo/EX_Stage/LampsLowPoly.msbo");
		lamps.material.texture		= {true, &lampTexture};
		lamps.material.illuminated	=
		lamps.material.shaded		= true;
		lamps.bake();
		// Set ground
		ground.extendFromBinaryFile("demo/EX_Stage/Ground.msbo");
		ground.material.texture = {true, &groundTexture};
		ground.material.illuminated	=
		ground.material.shaded		= true;
		ground.material.color = Vec4(.8, 1, .6, 1);
		ground.bake();
		// Set lights
		lights.extendFromBinaryFile("demo/EX_Stage/LampLights.msbo");
		lights.setRenderLayer(1);
		lights.material.color = Color::AZURE;
		lights.material.color += Vec4(0.33, 0.33, 0, 0);
		lights.material.fill = GL_LINE;
		lights.bake();
		// Set light sources
		lightSource.extendFromBinaryFile("demo/EX_Stage/LampLightCore.msbo");
		lightSource.bake();
		// Set culling
		lamps.material.culling			=
		ground.material.culling			=
		lightSource.material.culling	=
		lights.material.culling			= GL_FRONT;
		// Program Settings
		//Vec4 fogColor = Color::WHITE;
		Vec4 fogColor = Vec4(94, 93, 129, 1) / Vec4(Vec3(255),1);
		app->world3D.farFog = {true, 10, 30, fogColor};
		camera = &app->cam3D;
		camera->eye	= Vec3(	0	,	10	,	0	);
		camera->at	= Vec3(	0	,	-10	,	10	);
		camera->zFar = 50;
		camera->relativeToEye = true;
		app->layers[0].background = fogColor;
		app->bossAura.enabled = true;
		app->bossAura.effect = {
			true,
			200,
			Vec2(800, 450),
			Vec2(2.5),
			Vec4(0, 0, 0.25, 1),
			.75,
			true
		};
	}

	void onFrame(float delta) override {
		app->bossAura.effect.position.x = 800 + sin(app->getCurrentCycle() / 30.0) * 400;
		camera->eye.z += 1.0/10.0;
		if (camera->eye.z > 25.0)
			camera->eye.z -= 20.0;
		app->layers[1].color.w = sin((app->getCurrentCycle() / PI) / 2.0)/8.0 + 0.5;
	}
};

class TestGame: public AppType {
public:
	TestGame(
		unsigned int width,
		unsigned int height,
		std::string windowTitle,
		bool fullscreen = false
	): AppType(
		width,
		height,
		windowTitle,
		fullscreen
	) {

	}

	TestStage* stg;

	void onOpen() override {
		AppType::onOpen();
		stg = new TestStage();
		//stg->setup();
	}

	void onLogicFrame(float delta) override {
		if (input.getButtonDown(SDL_SCANCODE_ESCAPE)) {
			close();
		}
	}

	void onDrawBegin() override {
	}

	#define $rlayer(LAYER) ($layer(LAYER) / SUBLAYER_COUNT)
	void onLayerDrawBegin(size_t layerID) override {
		AppType::onLayerDrawBegin(layerID);
		switch (layerID / SUBLAYER_COUNT) {
		case $rlayer(WORLD):
			setCamera3D();
			setWorldMaterial3D();
			break;
		case $rlayer(PLAYER):
			break;
		case $rlayer(ENEMY):
			break;
		case $rlayer(OVERLAY):
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
		setWorldMaterial2D();
	}

	void onDrawEnd() override {
	}

	void onClose() override {
		AppType::onClose();
		delete stg;
	}
};

void runDemo(
	const unsigned int& w	= 1600,
	const unsigned int& h	= 900,
	const bool& fullscreen	= false
) {
	/*
	***********************
	*                     *
	*  Main Program Code  *
	*                     *
	***********************
	*/
	try {
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
		constexpr struct {
			const size_t performance	= 0xA1841308;
			const size_t balanced		= 0x381B4222;
			const size_t saver			= 0x8C5E7FDA;
		} powerScheme;
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
		#endif
		// If size >= screen size, automatic fullscreen for some reason (Why, SDL?)
		TestGame game(w, h, "Test Application: Game Stage", fullscreen);
		game.maxFrameRate = 60;
		game.run();
		$debug("Program done running!\n");
	} catch (std::runtime_error e) {
		Popup::errorDialog(e.what());
	}
	#ifdef _DEBUG_OUTPUT_
	$debug("\nAll done! Please press enter to close program!");
	getchar();
	#endif // _DEBUG_OUTPUT_
}
