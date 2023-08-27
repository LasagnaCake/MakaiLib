template<class P, class E>
struct ManagerSet {
	P* const player	= new P();
	E* const enemy	= new E();
	void destroy() {
		if (destroyed) return;
		destroyed = true;
		$debug(("Deleting enemy manager..."));
		delete enemy;
		$debug(("Deleting player manager..."));
		delete player;
	}
private:
	bool destroyed = false;
};

struct DanmakuManagerSet {
	ManagerSet<PlayerBulletManager, EnemyBulletManager>			bullet;
	ManagerSet<PlayerLineLaserManager, EnemyLineLaserManager>	lineLaser;
	ItemManager* const											item	= new ItemManager();
	void destroy() {
		if (destroyed) return;
		destroyed = true;
		$debug("Deleting bullet managers...");
		bullet.destroy();
		$debug("Deleting line laser managers...");
		lineLaser.destroy();
		$debug("Deleting item manager...");
		delete item;
	}
private:
	bool destroyed = false;
};

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

	DanmakuManagerSet managers;

	struct {
		PolarWarpEffect effect;
		bool enabled = false;
	} bossAura;

	float screenShake = 0;

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
			pPlayfield	= $cdt makeBounds(at, size * Vector2(1.25, 1.25)),
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
		if (layerID <= BOSS_SPELL_OVERLAY_TOP_LAYER) {
			if (bossAura.enabled)
				getLayerBuffer().material.polarWarp = bossAura.effect;
			else
				getLayerBuffer().material.polarWarp.enabled = false;
			if (screenShake)
				getLayerBuffer().uv.position += Vector3(getWindowScale() * VecMath::angleV2(Math::Random::real(0.0f, (float)TAU)) * screenShake);
		}
		switch (layerID / TRUE_SUBLAYER_COUNT) {
		case (WORLD_BOTTOM_LAYER / TRUE_SUBLAYER_COUNT):
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
		$debug("_( - v -)_");
		$debug("Clearing References...");
		if (managers.bullet.enemy		== enemyBulletManager)
			enemyBulletManager		= nullptr;
		if (managers.lineLaser.enemy	== enemyLineLaserManager)
			enemyLineLaserManager	= nullptr;
		if (managers.bullet.player		== playerBulletManager)
			playerBulletManager		= nullptr;
		if (managers.lineLaser.player	== playerLineLaserManager)
			playerLineLaserManager	= nullptr;
		if (managers.item				== itemManager)
			itemManager				= nullptr;
		$debug("Deleting Danmaku Managers ...");
		managers.destroy();
		$debug("Managers Deleted!");
		$debug("\\( > w <)/");
	}
};
