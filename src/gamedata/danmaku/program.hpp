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
		enemyBulletManager		= managers.bullet.enemy;
		enemyLineLaserManager	= managers.lineLaser.enemy;
		// Set managers (Player)
		playerBulletManager		= managers.bullet.player;
		playerLineLaserManager	= managers.lineLaser.player;
		// Set item manager
		itemManager				= managers.item;
	};

	struct {
		struct {
			EnemyBulletManager*		enemy	= new EnemyBulletManager();
			PlayerBulletManager*	player	= new PlayerBulletManager();
		} bullet;

		struct {
			EnemyLineLaserManager*	enemy	= new EnemyLineLaserManager();
			PlayerLineLaserManager*	player	= new PlayerLineLaserManager();
		} lineLaser;

		ItemManager*	item	= new ItemManager();
	} managers;

	struct {
		PolarWarpEffect effect;
		bool enabled = false;
	} bossAura;

	float& pointOfCollection = managers.item->poc;

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
		setGameBounds(screenSize);
	}

	void setGameBounds(Vector2 size, Vector2 position = Vector2(0)) {
		Vector2	screenSize	=
			Scene::camera.ortho.size.absolute()
		*	Vector2(1, -1)
		;
		Vector2	at			= (screenSize / 2.0) + position;
		BoxBounds2D
			ePlayfield	= $cdt makeBounds(at, size * Vector2(1.1, 1.1)),
			pPlayfield	= $cdt makeBounds(at, size * Vector2(2, 2)),
			board		= $cdt makeBounds(at, size);
		managers.item->poc = -screenSize.y / 3.0;
		managers.bullet.player->playfield = pPlayfield;
		managers.item->playfield =
		managers.bullet.enemy->playfield = ePlayfield;
		managers.bullet.enemy->board =
		managers.bullet.player->board = board;
	}

	void onClose() override {
		GameApp::onClose();
		destroyManagers();
	}

	void onLayerDrawBegin(size_t layerID) override {
		GameApp::onLayerDrawBegin(layerID);
		switch (layerID / SUBLAYER_COUNT) {
		case ($layer(WORLD) / SUBLAYER_COUNT):
			if (bossAura.enabled)
				getLayerBuffer().material.polarWarp = bossAura.effect;
			else
				getLayerBuffer().material.polarWarp.enabled = false;
			setWorldMaterial3D();
			setCamera3D();
			break;
		default:
			break;
		}
	}

	void onLayerDrawEnd(size_t layerID) override {
		GameApp::onLayerDrawEnd(layerID);
		setWorldMaterial2D();
		setCamera2D();
	}

private:
	void destroyManagers() {
		enemyBulletManager		= nullptr;
		enemyLineLaserManager	= nullptr;
		playerBulletManager		= nullptr;
		playerLineLaserManager	= nullptr;
		itemManager				= nullptr;
	}
};
