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
		// Set managers (Enemy)
		enemyBulletManager		= &managers.bullet.enemy;
		enemyLineLaserManager	= &managers.lineLaser.enemy;
		// Set managers (Player)
		playerBulletManager		= &managers.bullet.player;
		playerLineLaserManager	= &managers.lineLaser.player;
		// Set item manager
		itemManager				= &managers.item;
	};

	struct {
		struct {
			EnemyBulletManager		enemy;
			PlayerBulletManager		player;
		} bullet;

		struct {
			EnemyLineLaserManager	enemy;
			PlayerLineLaserManager	player;
		} lineLaser;

		ItemManager item;
	} managers;

	$cam Camera3D cam2D;
	$cam Camera3D cam3D{$vec3(0, 5, -10), $vec3(0, 0, 0)};

	size_t gameSeed = 0;

	void setCamera2D() {
		$scn camera = cam2D;
	}

	void setCamera3D() {
		$scn camera = cam3D;
	}

	virtual void onLoading() {}

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
					managers.bullet.enemy.haltProcedure =
					managers.lineLaser.enemy.haltProcedure =
					managers.bullet.player.haltProcedure =
					managers.lineLaser.player.haltProcedure =
					managers.item.haltProcedure = true;
					close();
				}
			}
		};
		std::thread secondary(subTask);
		// Create things
		managers.bullet.enemy.create();
		managers.lineLaser.enemy.create();
		managers.bullet.player.create();
		managers.lineLaser.player.create();
		managers.item.create();
		doneCreating = true;
		secondary.join();
		// Set playfield
		Vector2 screenSize = $scn camera.ortho.size.absolute();
		Vector2 screenPosition = Vector2(32, -32) * screenSpace;
		BoxBounds2D
			playfield = $cdt makeBounds(screenPosition, screenSize * Vector2(1.1, 1.1)),
			board = $cdt makeBounds(screenPosition, screenSize);
		managers.item.poc = -screenSize.y / 3.0;
		managers.item.playfield =
		managers.bullet.enemy.playfield =
		managers.bullet.player.playfield = playfield;
		managers.bullet.enemy.board =
		managers.bullet.player.board = board;
	}
};
