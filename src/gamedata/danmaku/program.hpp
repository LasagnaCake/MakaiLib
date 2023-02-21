namespace {
	using Makai::Program;
	WeakPointer<Program> mainProgram;
}

class DanmakuApp: public $mki Program {
public:
	DanmakuApp(
		unsigned int width,
		unsigned int height,
		std::string windowTitle,
		bool fullscreen = false,
		std::string bufferShaderPath = "shaders/framebuffer/compose.slf",
		std::string mainShaderPath = "shaders/base/base.slf",
		bool useMIDI = false
	) : Program (
			width,
			height,
			windowTitle,
			fullscreen,
			bufferShaderPath,
			false
	) {
		$debug("Creating Danmaku systems...");
		if (mainProgram.exists())
			throw std::runtime_error("Only one program can exist at a time!");
		mainProgram = this;
		// Create main shader
		SLF::SLFData data = SLF::parseFile(mainShaderPath);
		$mainshader.destroy();
		$mainshader.create(data);
		// Set managers (Enemy)
		managers.bullet.enemy		= enemyBulletManager		= new EnemyBulletManager();
		managers.lineLaser.enemy	= enemyLineLaserManager		= new EnemyLineLaserManager();
		// Set managers (Player)
		managers.bullet.player		= playerBulletManager		= new PlayerBulletManager();
		managers.lineLaser.player	= playerLineLaserManager	= new PlayerLineLaserManager();
		// Set item manager
		managers.item				= itemManager				= new ItemManager();
		$debug("Danmaku systems created!");
	};

	struct {
		struct {
			Pointer<EnemyBulletManager>		enemy;
			Pointer<PlayerBulletManager>	player;
		} bullet;

		struct {
			Pointer<EnemyLineLaserManager>	enemy;
			Pointer<PlayerLineLaserManager>	player;
		} lineLaser;

		Pointer<ItemManager> item;
	} managers;

	$cam Camera3D cam2D;
	$cam Camera3D cam3D{$vec3(0, 5, -10), $vec3(0, 0, 0)};

	$mat WorldMaterial world2D;
	$mat WorldMaterial world3D;

	size_t gameSeed = 0;

	inline void setCamera2D() {
		$scn camera = cam2D;
	}

	inline void setCamera3D() {
		$scn camera = cam3D;
	}

	inline void setWorldMaterial2D() {
		$mat setMaterial($mainshader, world2D);
	}

	inline void setWorldMaterial3D() {
		$mat setMaterial($mainshader, world3D);
	}

	virtual void onLoading() {}

	virtual ~DanmakuApp() {}

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
				onLoading();
				renderReservedLayer();
				if $event(SDL_QUIT) {
					managers.bullet.enemy->haltProcedure =
					managers.lineLaser.enemy->haltProcedure =
					managers.bullet.player->haltProcedure =
					managers.lineLaser.player->haltProcedure =
					managers.item->haltProcedure = true;
					close();
				}
			}
		};
		std::thread secondary(subTask);
		// Create things
		managers.bullet.enemy->create();
		managers.lineLaser.enemy->create();
		managers.bullet.player->create();
		managers.lineLaser.player->create();
		managers.item->create();
		doneCreating = true;
		secondary.join();
		// Set playfield
		Vector2 screenSize = $scn camera.ortho.size.absolute();
		Vector2 screenPosition = Vector2(32, -32) * screenSpace;
		BoxBounds2D
			playfield = $cdt makeBounds(screenPosition, screenSize * Vector2(1.1, 1.1)),
			board = $cdt makeBounds(screenPosition, screenSize);
		managers.item->poc = -screenSize.y / 3.0;
		managers.item->playfield =
		managers.bullet.enemy->playfield =
		managers.bullet.player->playfield = playfield;
		managers.bullet.enemy->board =
		managers.bullet.player->board = board;
	}

	void onClose() override {
		$debug("Deleting managers...");
		mainProgram.unbind();
		enemyBulletManager.destroy();
		enemyLineLaserManager.destroy();
		playerBulletManager.destroy();
		playerLineLaserManager.destroy();
		itemManager.destroy();
		$debug("Managers Deleted!");
	}
};

struct ProgramSetting {
	Vector2	resolution;
	float	framerate;
	bool	fullscreen;
};

#define USER_QUIT throw std::runtime_error("User quit the setup process!")

ProgramSetting queryProgramSettingsFromUser(bool use16by9 = false, bool letUserChooseFramerate = true, bool extendedFramerate = false) {
	StringList resList;
	std::vector<Vector2> resValue;
	for $each(res, (use16by9 ? $res set16x9 : $res set4x3)) {
		resList.push_back(res.key);
		resValue.push_back(res.value);
	}
	$debug(ENEMY_BULLET_COUNT);
	resList.push_back("Detect");
	Vector2 devSize = Makai::getDeviceSize();
	$debug(devSize.y * (4.0 / 3.0));
	$debug(devSize.y);
	$debug(devSize.y * (16.0 / 9.0));
	$debug(devSize.y);
	if (Popup::dialogBox(
		"Warning!",
		"Pressing escape on any of the next set of dialog boxes,\n"
		"including this one, will close the application.\n"
		"Do you understand?",
		Popup::Option::YES,
		SDL_MESSAGEBOX_WARNING
	) < 0) USER_QUIT;
	int winSize = Popup::dialogBox(
		std::string("App Configuration (1/") + (letUserChooseFramerate ? "3)" : "2)"),
		"Please select a window size.\n"
		"Selecting \"Detect\" will set to your screen's size.\n\n"
		"WARNING: Selecting \"Detect\" will set the application to fullscreen!\n",
		resList
	);
	if (winSize < 0) USER_QUIT;
	int fullscreen = 0;
	if (winSize != resList.size() - 1) {
		fullscreen = Popup::dialogBox(
			std::string("App Configuration (2/") + (letUserChooseFramerate ? "3)" : "2)"),
			"Would you like to run this application in fullscreen or windowed mode?",
			StringList{"Fullscreen", "Windowed"}
		);
	}
	if (fullscreen < 0) USER_QUIT;
	int framerate = 1;
	if (letUserChooseFramerate) {
		framerate = Popup::dialogBox(
			"App Configuration (3/3)",
			"Please select the maximum framerate (frames per second).\n"
			"Higher framerates require more powerful computers.\n"
			"If the program is running slow, try changing the framerate.",
			extendedFramerate
			? StringList{"10", "20", "30", "40", "50", "60"}
			: StringList{"30", "60"}
		);
		if (framerate < 0) USER_QUIT;
	}
	Vector2 window;
	if (winSize == resList.size() - 1) {
		window = devSize;
		window.x = window.y * (use16by9 ? 16.0/9.0 : 4.0/3.0);
	} else
		window = resValue[winSize];
	float frate = 0;
	if (extendedFramerate)
		frate =  10 + (framerate * 10);
	else frate = 30 + (framerate * 30);
	return ProgramSetting{
		window,
		frate,
		!fullscreen
	};
}


WeakPointer<DanmakuApp> getMainProgram() {
	return mainProgram.castedTo<DanmakuApp>();
}

#undef USER_QUIT
