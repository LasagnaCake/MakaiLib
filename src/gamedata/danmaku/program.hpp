#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
template<class P, class E>
#else
template<class P1, class P2, class E1, class E2>
#endif
struct ManagerSet {
	#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
	P* const player	= new P();
	E* const enemy	= new E();
	void destroy() {
		if (destroyed) return;
		destroyed = true;
		DEBUGLN(("Deleting enemy manager..."));
		delete enemy;
		DEBUGLN(("Deleting player manager..."));
		delete player;
	}
	#else
	P* const player1	= new P1();
	P* const player2	= new P2();
	E* const enemy1		= new E1();
	E* const enemy2		= new E2();
	void destroy() {
		if (destroyed) return;
		destroyed = true;
		DEBUGLN(("Deleting enemy managers..."));
		delete enemy1;
		delete enemy2;
		DEBUGLN(("Deleting player managers..."));
		delete player1;
		delete player2;
	}
	#endif
private:
	bool destroyed = false;
};

struct DanmakuManagerSet {
	#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
	ManagerSet<PlayerBulletManager, EnemyBulletManager>			bullet;
	ManagerSet<PlayerLineLaserManager, EnemyLineLaserManager>	lineLaser;
	ItemManager* const	item	= new ItemManager();
	#else
	ManagerSet<Player1BulletManager, Player2BulletManager, Enemy1BulletManager, Enemy2BulletManager>				bullet;
	ManagerSet<Player1LineLaserManager, Player2LineLaserManager, Enemy1LineLaserManager, Enemy2LineLaserManager>	lineLaser;
	ItemManager1* const	item1	= new ItemManager1();
	ItemManager2* const	item2	= new ItemManager2();
	#endif
	void destroy() {
		if (destroyed) return;
		destroyed = true;
		DEBUGLN("Deleting bullet managers...");
		bullet.destroy();
		DEBUGLN("Deleting line laser managers...");
		lineLaser.destroy();
		DEBUGLN("Deleting item manager...");
		#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
		delete item;
		#else
		delete item1;
		delete item2;
		#endif
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
		#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
		// Set managers (Enemy)
		enemyBulletManager		= managers.bullet.enemy;
		enemyLineLaserManager	= managers.lineLaser.enemy;
		// Set managers (Player)
		playerBulletManager		= managers.bullet.player;
		playerLineLaserManager	= managers.lineLaser.player;
		// Set item manager
		itemManager				= managers.item;
		#else
		// Set managers (Enemy)
		enemy1BulletManager		= managers.bullet.enemy1;
		enemy1LineLaserManager	= managers.lineLaser.enemy1;
		enemy2BulletManager		= managers.bullet.enemy2;
		enemy2LineLaserManager	= managers.lineLaser.enemy2;
		// Set managers (Player)
		player1BulletManager	= managers.bullet.player1;
		player1LineLaserManager	= managers.lineLaser.player1;
		player2BulletManager	= managers.bullet.player2;
		player2LineLaserManager	= managers.lineLaser.player2;
		// Set item managers
		itemManager1			= managers.item1;
		itemManager2			= managers.item2;
		#endif
	};

	DanmakuManagerSet managers;

	PolarWarpEffect bossAura;

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
				if SDL_EVENT(SDL_QUIT) {
					#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
					managers.bullet.enemy->haltProcedure =
					managers.lineLaser.enemy->haltProcedure =
					managers.bullet.player->haltProcedure =
					managers.lineLaser.player->haltProcedure =
					managers.item->haltProcedure = true;
					#else
					managers.bullet.enemy1->haltProcedure =
					managers.lineLaser.enemy1->haltProcedure =
					managers.bullet.player1->haltProcedure =
					managers.lineLaser.player1->haltProcedure =
					managers.bullet.enemy2->haltProcedure =
					managers.lineLaser.enemy2->haltProcedure =
					managers.bullet.player2->haltProcedure =
					managers.lineLaser.player2->haltProcedure =
					managers.item1->haltProcedure =
					managers.item2->haltProcedure = true;
					#endif
					close();
				}
			}
		};
		std::thread secondary(subTask);
		// Create things
		#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
		managers.bullet.enemy->create();
		managers.lineLaser.enemy->create();
		managers.bullet.player->create();
		managers.lineLaser.player->create();
		managers.item->create();
		#else
		managers.bullet.enemy1->create();
		managers.lineLaser.enemy1->create();
		managers.bullet.player1->create();
		managers.lineLaser.player1->create();
		managers.bullet.enemy2->create();
		managers.lineLaser.enemy2->create();
		managers.bullet.player2->create();
		managers.lineLaser.player2->create();
		managers.item1->create();
		managers.item2->create();
		#endif
		doneCreating = true;
		secondary.join();
		// Set playfield
		Vector2 screenSpace = getWindowScale();
		Vector2 screenSize = Scene::camera.ortho.size.absolute();
		setGameBounds(screenSize);
	}

	#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
	void setGameBounds(Vector2 size, Vector2 position = 0) {
		Vector2	screenSize	=
			Scene::camera.ortho.size.absolute()
		*	Vector2(1, -1)
		;
		Vector2	at			= (screenSize / 2.0) + position;
		BoxBounds2D
			ePlayfield	= CollisionData::makeBounds(at, size * Vector2(1.1, 1.1)),
			pPlayfield	= CollisionData::makeBounds(at, size * Vector2(1.25, 1.25)),
			board		= CollisionData::makeBounds(at, size);
		managers.item->poc = -screenSize.y / 3.0;
		managers.bullet.player->playfield = pPlayfield;
		managers.item->playfield =
		managers.bullet.enemy->playfield = ePlayfield;
		managers.bullet.enemy->board =
		managers.bullet.player->board = board;
	}
	#else
	void setGameBounds(Vector2 size, Vector2 pos1 = 0, vector2 pos2 = 0) {
		Vector2	screenSize	=
			Scene::camera.ortho.size.absolute()
		*	Vector2(1, -1)
		;
		Vector2	at1			= (screenSize / 2.0) + pos1;
		Vector2	at2			= (screenSize / 2.0) + pos2;
		BoxBounds2D
			e1Playfield	= CollisionData::makeBounds(at1, size * Vector2(1.1, 1.1)),
			e2Playfield	= CollisionData::makeBounds(at2, size * Vector2(1.1, 1.1)),
			p1Playfield	= CollisionData::makeBounds(at1, size * Vector2(1.25, 1.25)),
			p2Playfield	= CollisionData::makeBounds(at2, size * Vector2(1.25, 1.25)),
			board1		= CollisionData::makeBounds(at1, size);
			board2		= CollisionData::makeBounds(at2, size);
		managers.item1->poc =
		managers.item2->poc = -screenSize.y / 3.0;
		// Set 1
		managers.bullet.player1->playfield = p1Playfield;
		managers.item1->playfield =
		managers.bullet.enemy1->playfield = e1Playfield;
		managers.bullet.enemy1->board =
		managers.bullet.player1->board = board1;
		// Set 2
		managers.bullet.player2->playfield = p2Playfield;
		managers.item2->playfield =
		managers.bullet.enemy2->playfield = e2Playfield;
		managers.bullet.enemy2->board =
		managers.bullet.player2->board = board2;
	}
	#endif

	void onClose() override {
		GameApp::onClose();
		destroyManagers();
	}

	void onLayerDrawBegin(size_t layerID) override {
		GameApp::onLayerDrawBegin(layerID);
		switch (layerID / TRUE_SUBLAYER_COUNT) {
		case (WORLD_BOTTOM_LAYER / TRUE_SUBLAYER_COUNT):
			setWorldMaterial3D();
			setCamera3D();
			break;
		default:
			break;
		}
		if (layerID <= BOSS_SPELL_OVERLAY_TOP_LAYER) {
			getLayerBuffer().material.polarWarp = bossAura;
			if (screenShake)
				getLayerBuffer().material.uvShift += Vector2(getWindowScale() * VecMath::angleV2(Math::Random::real<float>(0.0f, TAU)) * screenShake);
		}
	}

	void onLayerDrawEnd(size_t layerID) override {
		GameApp::onLayerDrawEnd(layerID);
		setWorldMaterial2D();
		setCamera2D();
	}

private:
	void destroyManagers() {
		DEBUGLN("_( - v -)_");
		DEBUGLN("Clearing References...");
		#ifndef MAKAILIB_DANMAKU_PHANTASMAGORIA_GAME
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
		#else
		// Set 1
		if (managers.bullet.enemy1		== enemy1BulletManager)
			enemy1BulletManager		= nullptr;
		if (managers.lineLaser.enemy1	== enemy1LineLaserManager)
			enemy1LineLaserManager	= nullptr;
		if (managers.bullet.player1		== player1BulletManager)
			player1BulletManager	= nullptr;
		if (managers.lineLaser.player1	== player1LineLaserManager)
			player1LineLaserManager	= nullptr;
		if (managers.item1				== item1Manager)
			item1Manager			= nullptr;
		// Set 2
		if (managers.bullet.enemy2		== enemy2BulletManager)
			enemy2BulletManager		= nullptr;
		if (managers.lineLaser.enemy2	== enemy2LineLaserManager)
			enemy2LineLaserManager	= nullptr;
		if (managers.bullet.player2		== player2BulletManager)
			player2BulletManager	= nullptr;
		if (managers.lineLaser.player2	== player2LineLaserManager)
			player2LineLaserManager	= nullptr;
		if (managers.item2				== item2Manager)
			item2Manager			= nullptr;
		#endif
		DEBUGLN("Deleting Danmaku Managers ...");
		managers.destroy();
		DEBUGLN("Managers Deleted!");
		DEBUGLN("\\( > w <)/");
	}
};
