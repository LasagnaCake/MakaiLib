class DanmakuApp: public GameApp {
public:
	DanmakuApp(
		unsigned int width,
		unsigned int height,
		std::string windowTitle,
		bool fullscreen = false,
		bool useMIDI = false,
		std::string bufferShaderPath = "shaders/framebuffer/compose.slf",
		std::string mainShaderPath = "shaders/base/base.slf"
	) : GameApp (
			width,
			height,
			windowTitle,
			fullscreen,
			useMIDI,
			bufferShaderPath,
			mainShaderPath
	) {
		// Set managers (Enemy)
		enemyBulletManager		= managers.bullet.enemy		= new EnemyBulletManager();
		enemyLineLaserManager	= managers.lineLaser.enemy	= new EnemyLineLaserManager();
		// Set managers (Player)
		playerBulletManager		= managers.bullet.player	= new PlayerBulletManager();
		playerLineLaserManager	= managers.lineLaser.player	= new PlayerLineLaserManager();
		// Set item manager
		itemManager				= managers.item				= new ItemManager();
	};

	struct {
		struct {
			EnemyBulletManager*		enemy;
			PlayerBulletManager*	player;
		} bullet;

		struct {
			EnemyLineLaserManager*	enemy;
			PlayerLineLaserManager*	player;
		} lineLaser;

		ItemManager*	item;
	} managers;

	virtual void onLoading() {}

	virtual ~DanmakuApp() {
		destroyManagers();
	}

	void onOpen() override {
		GameApp::onOpen();
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
		Vector2 screenSpace = getWindowScale();
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
		GameApp::onClose();
		destroyManagers();
	}

private:
	void destroyManagers() {
		delete managers.bullet.enemy;
		delete managers.lineLaser.enemy;
		delete managers.bullet.player;
		delete managers.lineLaser.player;
		delete managers.item;
		enemyBulletManager		= nullptr;
		enemyLineLaserManager	= nullptr;
		playerBulletManager		= nullptr;
		playerLineLaserManager	= nullptr;
		itemManager				= nullptr;
	}
};
